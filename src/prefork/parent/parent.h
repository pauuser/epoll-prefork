#ifndef PARENT_H_INCLUDED
#define PARENT_H_INCLUDED
#define MAX_CHILD_COUNT 1000

#include <sys/types.h>

typedef enum {
    SERVER_ITEM_AVAILABLE,
    SERVER_ITEM_BUSY,
    SERVER_ITEM_DEAD
} ServerItemState;

struct server_item{
    ServerItemState state;
    pid_t pid;
    int served;
};
typedef struct server_item server_item;

void init_server();

void check_children();

void stop_server();

#endif // PARENT_H_INCLUDED
