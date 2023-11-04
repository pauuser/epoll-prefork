#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "http_writer.h"

void http_write_status_line(int fd, const char *http_version, const char *status_code, const char *error_phrase)
{
    write(fd, http_version, strlen(http_version));
    write(fd, " ", 1);
    write(fd, status_code, strlen(status_code));
    write(fd, " ", 1);
    write(fd, error_phrase, strlen(error_phrase));
    write(fd, "\r\n", 2);
}

void http_write_header(int fd, const char *key, const char *value)
{
    write(fd, key, strlen(key));
    write(fd, ": ", 2);
    write(fd, value, strlen(value));
    write(fd, "\r\n", 2);
}

void http_write_int_header(int fd, const char *key, int value)
{
    char data[32];
    sprintf(data, "%d", value);
    http_write_header(fd, key, data);
}

void http_empty_line(int fd)
{
    write(fd, "\r\n", 2);
}
