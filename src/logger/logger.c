#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <semaphore.h> 
#include "logger.h"

static FILE* log_file = NULL;
static sem_t semaphore;

void init_logger(const char* filename) 
{
    sem_init(&semaphore, 0, 1);

    log_file = fopen(filename, "a");

    if (log_file == NULL) 
    {
        printf("Failed to open log file.\n");
    }
}

const char* get_level_string(log_level_t level)
{
    switch (level)
    {
        case INFO:
            return "INFO";

        case WARNING:
            return "WARNING";

        case ERROR:
            return "ERROR";

        default:
            break;
    }
}

void log(enum LOG_LEVEL level, const char *fmt, ...) 
{
    char timestamp[30];
    time_t current_time;
    time(&current_time);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
    
    va_list ptr;
    va_start(ptr, fmt);
    const char* level_string = get_level_string(level);

    sem_wait(&semaphore); 
    if (log_file)
    {
        fprintf(log_file, "[%s] [%s] [pid %d] ", timestamp, level_string, getpid());
        vfprintf(log_file, fmt, ptr);
        fprintf(log_file, "\n");
        fflush(log_file);
    }
    else
    {
        printf("[%s] [%s] ", timestamp, level_string);
        vprintf(fmt, ptr);
        printf("\n");
    }
    sem_post(&semaphore);

    va_end(ptr);
}

void close_logger() 
{
    if (log_file != NULL) 
    {
        fclose(log_file);
        log_file = NULL;
    }
}