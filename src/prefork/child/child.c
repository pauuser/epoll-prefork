#include "child.h"

int get_version_errors(http_parse_request* request, int fd, server_item *item)
{
    int errors = 0;

    if (stricmp(request->http_version, "http/1.0") != 0 &&
        stricmp(request->http_version, "http/1.1") != 0)
    {

        STATUS_400(fd, HTTP_1_0);
        write_error_headers(fd);
        END_CLIENT(item, fd, request);
        
        errors += 1;
    }

    return errors;
}

int get_method_not_allowed_errors(http_parse_request* request, int fd, server_item *item)
{
    int errors = 0;

    if (stricmp(request->method, "get") != 0 &&
        stricmp(request->method, "head") != 0)
    {
        STATUS_405(fd, HTTP_1_0);
        write_error_headers(fd);
        END_CLIENT(item, fd, request);
        
        errors += 1;
    }

    return errors;
}

int check_state(http_parse_request* request, int fd, server_item* item)
{
    int errors = 0;

    if (request->state != STATE_HEADER_DONE)
    {
        if (request->state == STATE_ERROR)
        {
            STATUS_400(fd, HTTP_1_0);
            write_error_headers(fd);
            END_CLIENT(item, fd, request);
        }
        else
        {
            END_CLIENT(item, fd, request); // TODO: ??????
        }
        
        errors += 1;
    }
    
    return errors;
}

int check_request_errors(http_parse_request* request, int fd, server_item* item)
{
    int errors = 0;

    errors += check_state(request, fd, item) + 
        get_version_errors(request, fd, item) + 
        get_method_not_allowed_errors(request, fd, item);

    return errors;
}

config_host* get_host(int fd, http_parse_request* request, server_item* item)
{
    char *host_param = http_parser_find_param(request, "host");

    config_host *host = NULL;
    if (host_param != NULL)
    {
        host = find_host(host_param);
    }
    
    if (host_param == NULL || host == NULL)
    {
        log(INFO, "Responded 404: host not found");
        STATUS_404(fd, HTTP_1_0);
        write_error_headers(fd);
        END_CLIENT(item, fd, request);
    }

    return host;
}

void remove_params(char *path)
{
    char *tmp = path;
    while (*tmp != '\0' && *tmp != '?' && *tmp != '#') 
        tmp++;
    *tmp = '\0';
}

void check_root_file(http_parse_request* request)
{
    if (strcmp(request->path, "/") == 0)
    {
        memset(request->path, '\0', sizeof(request->path));
        strcpy(request->path, "index.html");
    }
}

void process_events(struct epoll_event* events, 
    ssize_t events_count, 
    int server_socket, 
    server_item *item, 
    int* processed_clients)
{
    for (int i = 0; i < events_count; i++) 
    {
        struct sockaddr_storage client_addr;
        socklen_t client_addrlen = sizeof client_addr;

        /* accept up all connections. we're non-blocking, -1 == no more connections */
        if (events[0].events & EPOLLIN) 
        {
            int fd = accept(server_socket, (struct sockaddr *)&client_addr, &client_addrlen);
            if (fd < 0)
            {
                log(ERROR, "Server thread FAILED to proccess accept with EAGAIN");
                continue;
            }

            item->state = SERVER_ITEM_BUSY;
            http_parse_request* request = read_request(fd);
            
            (*processed_clients)++;

            if (check_request_errors(request, fd, item) > 0)
                continue;

            config_host *host;
            if (!(host = get_host(fd, request, item)))
                continue;

            check_root_file(request);
            remove_params(request->path);

            log(INFO, "Returning file %s", request->path);
            respond_file(fd, host, request);
            END_CLIENT(item, fd, request);

            item->state = SERVER_ITEM_AVAILABLE;

            close(fd);
        }
    }
}

void process_client(int server_socket, server_item *item)
{
    set_parent_death_signal();

    config *config  = config_get();
    
    int epoll_fd = configure_epoll(server_socket);
    
    start_processing_loop(epoll_fd, config, server_socket, item);
}

void set_parent_death_signal()
{
    prctl(PR_SET_PDEATHSIG, SIGHUP);
}

void start_processing_loop(int epoll_fd, config* config, int server_socket, server_item *item)
{
    int processed_clients = 0;
    while (1)
    {
        if (processed_clients >= config->child_max_queries)
        {
            log(INFO, "Clients limit served, process %d is shutting down", item->pid);
            exit(0);
        }

        struct epoll_event events[10];
        ssize_t events_count = get_events(epoll_fd, events, item);
        
        log(INFO, "Server thread woke up with %lu events", events_count);

        process_events(events, events_count, server_socket, item, &processed_clients); 
    }
}

ssize_t get_events(int epoll_fd, struct epoll_event* events, server_item *item)
{
    ssize_t events_count;
    events_count = epoll_wait(epoll_fd, events, 10, -1);

    if (events_count <= 0) 
    {
        log(ERROR, "Process with pid=%d wakes up with no events, shutting down", item->pid);
        exit(0);
    }

    return events_count;
}

void build_path_to_file(char* fullpath, config_host *host, char *path)
{
    int root_len = strlen(host->root), path_len = strlen(path);

    char *curr = fullpath;
    strcpy(curr, host->root);

    curr += root_len;
    if (host->root[root_len - 1] != '/')
    {
        *curr = '/'; 
        curr++;
    }

    strcpy(curr, path);
    curr += path_len;
    *curr = '\0';

    log(INFO, "Full path built: %s", fullpath);
}

static void ok(int fd, FILE* file, http_parse_request* request)
{
    STATUS_200(fd, HTTP_1_0);
    write_base_headers(fd);
    PARAM_CONTENT_LENGTH(fd, file_length(file));
    PARAM_CONTENT_TYPE(fd, detect_mime_type(request->path, file));
    
    http_empty_line(fd);
    if (stricmp(request->method, "head") != 0)
    {
        write_file(file, fd);
    }

    log(INFO, "Responded 200 for path=%s", request->path);
}

void respond_file(int fd, config_host *host, http_parse_request* request)
{
    char fullpath[512];
    int root_len = strlen(host->root), path_len = strlen(request->path);

    if (root_len < 1 || path_len < 1 || root_len + path_len + 2 >= 512)
    {
        log(INFO, "Responded 400: path_len=%d, root_len=%d, path=%s", path_len, root_len, request->path);
        STATUS_400(fd, HTTP_1_0);
        write_error_headers(fd);
        return;
    }

    build_path_to_file(fullpath, host, request->path);

    FILE *file = fopen(fullpath, "rb");
    if (file == NULL)
    {
        log(INFO, "Reponded 404: file %s not found", fullpath);
        STATUS_404(fd, HTTP_1_0);
        write_error_headers(fd);
        return;
    }

    ok(fd, file, request);
}

http_parse_request* read_request(int fd)
{
    char buffer[1024];
    http_parse_request* request = http_parse_create_request();
    while (1)
    {
        size_t c = read(fd, buffer, 1024);
        if (c <= 0)
        {
            break;
        }
        http_proceed_request(request, buffer, c);
        if (request->state == STATE_ERROR || request->state == STATE_HEADER_DONE)
        {
            break;
        }
    }
    log(INFO, "Read request successfully");

    return request;
}

/*
* Copy file from host to socket
*/
void write_file(FILE *file_d, int socket_d)
{
    char data[SEND_BUFFER_SIZE];

    size_t n = 0;
    while ((n = fread(data, sizeof(char), SEND_BUFFER_SIZE, file_d)) > 0)
    {
        write(socket_d, data, n);
    }
}

void write_base_headers(int fd)
{
    PARAM_DATE(fd, current_time());
    PARAM_SERVER(fd, SERVER_NAME);
    PARAM_CONNECTION(fd, "closed");
}

void write_error_headers(int fd)
{
    write_base_headers(fd);
    PARAM_CONTENT_LENGTH(fd, 0);
    PARAM_CONTENT_TYPE(fd, "text/html");
    http_empty_line(fd);
}

int get_epoll_fd()
{
    log(INFO, "Creating epoll fd");

    int epoll_fd = epoll_create1(0);

    if (epoll_fd < 0) 
    {
        log(ERROR, "Unable to get epoll_fd: epoll_create1 failed");
        exit(EXIT_FAILURE);
    }

    log(INFO, "Epoll fd=%d received", epoll_fd);

    return epoll_fd;
}

int configure_epoll(int server_socket)
{
    log(INFO, "Configuring epoll with server_socket=%d", server_socket);

    int epoll_fd = get_epoll_fd();

    struct epoll_event event = {0};
    event.data.ptr = (void *)((uintptr_t)server_socket);
    event.events = EPOLLIN | EPOLLERR | EPOLLEXCLUSIVE;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event) != 0) 
    {
        log(ERROR, "Unable to set socket for epoll: epoll_ctl failed");
        exit(EXIT_FAILURE);
    }

    log(INFO, "Epoll configured successfully");

    return epoll_fd;
}
