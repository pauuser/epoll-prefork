#ifndef PARENT_H_INCLUDED
#define PARENT_H_INCLUDED
#define MAX_CHILD_COUNT 1000

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "child.h"
#include "utils.h"
#include "config.h"
#include "logger.h"
#include "common.h"

void init_server();

void check_children();

void stop_server();

#endif // PARENT_H_INCLUDED
