#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <netinet/in.h>
#include <stdio.h>

#if !defined(WIN32)
#define stricmp(a, b) strcasecmp(a,b)
#endif

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

int is_wspace(char c);

char *rtrim(char *in);

char *ltrim(char *in);

void die_with_error(char *error_text);

char* copy_till(char *src, char *dest, char till, int limit);

int str_to_sockaddr_ipv4(char *src, struct sockaddr_in *dst);

int file_length(FILE *file);

const char * current_time();

#endif // UTILS_H_INCLUDED
