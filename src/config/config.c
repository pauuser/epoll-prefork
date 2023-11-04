#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "./parser/config_parser.h"
#include "../utils/utils.h"

struct config* current_config =  NULL;

char *read_host(struct config *config, char *buf);
char *read_block(struct config *config, char *buf);
void set_default(config *config);

struct config *config_get()
{
    if (!current_config)
    {
        current_config = (struct config *)malloc(sizeof(struct config));

        set_default(current_config);
    }

    return current_config;
}

void set_default(config *config)
{
    config->bind_to = "127.0.0.1:80";
    config->child_max_queries = 100;
    config->min_children = 1;
    config->max_children = 2;
    config->hosts_count = 0;
}

void config_read_from_file(FILE *file)
{
    int length = file_length(file);
    char *buffer = malloc (length+1);
    char *orig_buffer = buffer;
    memset(buffer, 0, length+1);
    fread (buffer, 1, length, file);

    config *new_conf = (config *)malloc(sizeof(config));
    set_default(new_conf);
    while(*buffer)
    {
        buffer = ltrim(buffer);
        buffer = read_block(new_conf, buffer);
        buffer = ltrim(buffer);
    }
    free(orig_buffer);
    current_config = new_conf;
}

char *read_block(config *config, char *buf)
{
    buf = ltrim(buf);
    char ident[64], str[STRING_BUFFER];
    buf = read_ident(buf, ident, 64 - 1);

    buf = ltrim(buf);
    if(strcmp(ident, "min_children") == 0)
    {
        buf = read_int(buf, &(config->min_children));
    }
    else if(strcmp(ident, "max_children") == 0)
    {
        buf = read_int(buf, &(config->max_children));
    }
    else if(strcmp(ident, "child_max_queries") == 0)
    {
        buf = read_int(buf, &(config->child_max_queries));
    }
    else if(strcmp(ident, "bind") == 0)
    {
        buf = read_string(buf, &str, STRING_BUFFER - 1);
        config->bind_to = strdup(str);
    }
    else if(strcmp(ident, "host") == 0)
    {
        buf = ltrim(consume(ltrim(buf), "{"));
        buf = read_host(config, buf);
        buf = consume(ltrim(buf), "}");
    }
    else
    {
        die_with_error("unknown identifier");
    }

    buf = consume(ltrim(buf), ";");
    return buf;
}

char *read_host(config *config, char *buf)
{
    int host_id = config->hosts_count;
    if (host_id >= HOSTS_LIMIT)
    {
        die_with_error("hosts limit reached");
    }
    config->hosts_count++;

    char ident[64], str[STRING_BUFFER];
    while (1)
    {
        buf = ltrim(buf);
        buf = read_ident(buf, ident, 64 - 1);

        buf = ltrim(buf);
        if(strcmp(ident, "mask") == 0)
        {
            do
            {
                buf = read_string(buf, &str, STRING_BUFFER - 1);

                mask_list *new_mask = (mask_list *)malloc(sizeof(mask_list));
                new_mask->mask = strdup(str);
                new_mask->next = config->hosts[host_id].mask;
                config->hosts[host_id].mask = new_mask;

                buf = ltrim(buf);
            }
            while(*buf != ';');
        }
        else if(strcmp(ident, "root") == 0)
        {
            buf = read_string(buf, &str, STRING_BUFFER - 1);
            config->hosts[host_id].root = strdup(str);
        }
        else if(strlen(ident) == 0)
        {
            break;
        }
        else
        {
            die_with_error("unknown identifier ");
        }

        buf = consume(ltrim(buf), ";");
    }
    return buf;
}

config_host* find_host(char *host)
{
    // @TODO implement real host masks
    if (current_config == NULL)
    {
        return NULL;
    }

    int i;
    for (i = 0; i < current_config->hosts_count; i++)
    {
        mask_list *mask = current_config->hosts[i].mask;
        while (mask != NULL)
        {
            if (strcmp(mask->mask, "*") == 0 || stricmp(mask->mask, host) == 0)
            {
                return &(current_config->hosts[i]);
            }
            mask = mask->next;
        }
    }

    return NULL;
}
