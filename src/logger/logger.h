#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <semaphore.h> 

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