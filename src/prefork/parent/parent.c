#include "parent.h"

static server_item *children;
int used_children = 0;
int server_socket;
int max_conn = 512;

void sigchld_handler(int sig);

void init_server()
{
    /* 
    * Setting SIGCHLD handler
    */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigchld_handler;
    sigaction(SIGCHLD, &sa, NULL);

    // allocating shared memory
    children = mmap(NULL, sizeof (server_item) * (MAX_CHILD_COUNT + 1),
        PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    for(int i = 0; i < MAX_CHILD_COUNT; i++)
    {
        children[i].pid = 0;
        children[i].state = SERVER_ITEM_DEAD;
    }

    config *config = config_get();
    bind_server(config);
    chroot(config->hosts[0].root);

    int base_fork = config->min_children;
    if (base_fork < 1 || base_fork > MAX_CHILD_COUNT)
    {
        die_with_error("min_children must be between 1 and MAX_CHILD_COUNT");
    }

    for (int i = 0; i < base_fork; i++)
    {
        fork_child(children + i);
    }

    used_children = base_fork;
}

void bind_server(config *conf)
{
    struct sockaddr_in addr;
    if (str_to_sockaddr_ipv4(conf->bind_to, &addr) == 0)
    {
        die_with_error("bind: parse failed");
    }
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) 
    {
        die_with_error("socket failed");
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    int bres;
    if ((bres = bind(server_socket, (struct sockaddr *) &addr, sizeof(addr))) < 0) 
    {
        die_with_error("bind failed");
    }

    log(INFO, "Listening on %s", conf->bind_to);

    listen(server_socket, max_conn);
}

void fork_child(server_item *item)
{
    if (item->state != SERVER_ITEM_DEAD)
    {
        return; // child is alive
    }

    item->state = SERVER_ITEM_AVAILABLE;

    pid_t pid = fork();
    if (pid < 0)
    {
        die_with_error("Fork failed");
    }
    else if (pid > 0)
    {
        // parent process
        item->pid = pid;
    }
    else if (pid == 0)
    {
        // child process
        process_client(server_socket, item);
        exit(0);
    }
}

void check_children()
{
    config *config = config_get();
    int alive_count = 0, available_count = 0;

    /*
    * Get current children pool state
    */
    for(int i = 0; i < used_children; i++)
    {
        if (children[i].state == SERVER_ITEM_DEAD)
        {
            continue;
        }

        alive_count++;
        if (children[i].state == SERVER_ITEM_AVAILABLE)
        {
            available_count++;
        }
    }

    /*
    * Count how many children should be added
    */
    int add_count = 0;
    if (alive_count < config->min_children)
    {
        add_count = config->min_children - alive_count;
    }
    if (available_count == 0 && 
        add_count == 0 && 
        alive_count + 1 < config->max_children && 
        alive_count + 1 < MAX_CHILD_COUNT)
    {
        add_count = 1;
    }

    /*
    * Make dead children alive again
    */
    for (int i = 0; i < used_children && add_count > 0; i++)
    {
        if (children[i].state == SERVER_ITEM_DEAD)
        {
            fork_child(children + i);
            add_count--;
            available_count++;
        }
    }

    /*
    * Create new children
    */
    if (add_count > 0)
    {
        for(int i = used_children; i < (used_children + add_count); i++)
        {
            fork_child(children + i);
        }
        used_children += add_count;
    }
}

void stop_server()
{
    if (server_socket > 0)
    {
        close(server_socket);
        server_socket = 0;
    }

    /*
    * Kill child processes
    */
    for(int i = 0; i < used_children; i++)
    {
        if (children[i].state != SERVER_ITEM_DEAD)
        {
            kill(children[i].pid, SIGKILL);
        }
    }
}

/*
* Handle child's death gracefully
*/
void sigchld_handler(int sig)
{
    pid_t p;
    int status;

    while ((p = waitpid(-1, &status, WNOHANG)) != -1)
    {
        for(int i = 0; i < used_children; i++)
        {
            if (children[i].pid == p)
            {
                children[i].state = SERVER_ITEM_DEAD;
                break;
            }
        }
    }
}

