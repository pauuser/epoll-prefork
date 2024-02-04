#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <semaphore.h> 
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>

#define LOGGER_SLEEP_TIME_USEC 10000L // 10 ms

typedef enum LOG_LEVEL 
{
    WARNING = 0,
    INFO = 1,
    ERROR = 2
} log_level_t;

void init_logger(const char* filename);

void log(log_level_t level, const char *fmt, ...);

void close_logger();

#endif