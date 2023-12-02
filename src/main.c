#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./config/config.h"
#include "./prefork/parent/parent.h"
#include "./logger/logger.h"
#include <sys/prctl.h>
#include <signal.h>

#define CHILD_CHECK_INTERVAL_USEC 50000L // 50 ms

void read_config(){
    FILE *file = fopen("./server.cfg", "r");
    config_read_from_file(file);
}

static void quit_handler(int signo) {
    stop_server();
    exit(0);
}

static void set_signal_handlers()
{
    signal(SIGHUP, quit_handler);
    signal(SIGINT, quit_handler);
    signal(SIGSTOP, quit_handler);
    signal(SIGABRT, quit_handler);
    signal(SIGKILL, quit_handler);
    signal(SIGSEGV, quit_handler);
    signal(SIGTERM, quit_handler);
    signal(SIGQUIT, quit_handler);
}

int main()
{
    init_logger("prefork.log");
    
    read_config();
    init_server();

    set_signal_handlers();

    while (1)
    {
        check_children();
        usleep(CHILD_CHECK_INTERVAL_USEC);
    }

    return 0;
}

