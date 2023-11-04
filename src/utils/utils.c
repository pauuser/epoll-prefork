#include "utils.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdlib.h>
#include "../logger/logger.h"

int is_wspace(char c)
{
    switch(c){
    case ' ':
    case '\t':
    case '\n':
    case '\r':
        return 1;
    }
    return 0;
}

char *ltrim(char *in)
{
    while(*in != 0 && is_wspace(*in)) 
        in++;

    return in;
}

char *rtrim(char *in)
{
    char *tmp = in + strlen(in) - 1;
    while(tmp >= in && is_wspace(*tmp)) 
        *tmp-- = '\0';
        
    return in;
}

char* copy_till(char *src, char *dest, char till, int limit){
    if (limit <= 0)
    {
        return 0;
    }

    int pos = 0;
    while (*src && *src != till)
    {
        *dest++ = *src++;
        if (pos >= limit)
        {
            break;
        }
    }

    *dest = '\0';
    if (*src == '\0' || *src == till)
    {
        return src;
    }

    return 0;
}

int str_to_sockaddr_ipv4(char *src, struct sockaddr_in *dst)
{
    bzero((char *) dst, sizeof(struct sockaddr_in));
    src = ltrim(rtrim(src));

    char addr[32];
    char port[16];
    src = copy_till(src, addr, ':', 31);
    if (src == 0 || strlen(addr) < 8)
    {
        return 0;
    }

    src++;

    src = copy_till(src, port, '\0', 15);
    if (src == 0 || strlen(port) < 1)
    {
        return 0;
    }

    char *addr_p = ltrim(rtrim(addr));
    char *port_p = ltrim(rtrim(port));

    if (!inet_pton(AF_INET, addr_p, &(dst->sin_addr)))
    {
        return 0;
    }

    char *ep;
    int n = strtol(port_p, &ep, 10);
    if (*ep != 0 || n <= 0)
    {
        return 0;
    }

    dst->sin_port = htons(n);
    dst->sin_family = AF_INET;

    return 1;
}

int file_length(FILE *file)
{
    fseek(file, 0, SEEK_END);
    int s = ftell(file);
    fseek(file, 0, SEEK_SET);

    return s;
}

const char* current_time()
{
    static char local_copy[64];
    memset(local_copy, '\0', sizeof(local_copy));
    time_t curtime  = time(NULL);
    strcpy(local_copy, asctime(localtime(&curtime)));
    rtrim(local_copy);

    return &local_copy;
}

void die_with_error(char *error_text)
{
    log(ERROR, error_text);
    exit(EXIT_FAILURE);
}
