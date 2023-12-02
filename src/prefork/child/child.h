#ifndef CHILD_H_INCLUDED
#define CHILD_H_INCLUDED

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/epoll.h>

#include "common.h"
#include "http_parser.h"
#include "config.h"
#include "mime.h"
#include "http_writer.h"
#include "utils.h"
#include "logger.h"

#define SERVER_NAME "epoll-prefork"
#define HTTP_1_0 "HTTP/1.0"
#define SEND_BUFFER_SIZE 1024

#define END_CLIENT(item, client_fd, request) { item->state = SERVER_ITEM_AVAILABLE; close(client_fd); http_parse_free_request(request); }

void process_client(int server_socket, server_item *item);

void write_file(FILE *file_d, int socket_d);

void write_error_headers(int fd);

void write_base_headers(int fd);

void respond_file(int fd, config_host *host, http_parse_request* request);

int get_epoll_fd();

int configure_epoll(int server_socket);

void set_parent_death_signal();

ssize_t get_events(int epoll_fd, struct epoll_event* events, server_item *item);

void start_processing_loop(int epoll_fd, config* config, int server_socket, server_item *item);

http_parse_request* read_request(int fd);

#endif // CHILD_H_INCLUDED
