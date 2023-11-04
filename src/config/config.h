#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <sys/types.h>
#include <stdio.h>

#define HOSTS_LIMIT 128
#define STRING_BUFFER 512

struct mask_list{
    struct mask_list *next;
    char *mask;
};
typedef struct mask_list mask_list;

struct config_host{
    mask_list *mask;
    char *root;
};
typedef struct config_host config_host;

struct config{
    int min_children;
    int max_children;
    int child_max_queries;
    char *bind_to;
    config_host hosts[HOSTS_LIMIT];
    int hosts_count;
};
typedef struct config config;

config *config_get();
void config_read_from_file(FILE *file);
config_host* find_host(char *host);

#endif // CONFIG_H_INCLUDED
