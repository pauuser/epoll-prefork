#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

typedef enum 
{
    SERVER_ITEM_AVAILABLE,
    SERVER_ITEM_BUSY,
    SERVER_ITEM_DEAD
} 
ServerItemState;

struct server_item
{
    ServerItemState state;
    pid_t pid;
    int served;
};
typedef struct server_item server_item;

#endif