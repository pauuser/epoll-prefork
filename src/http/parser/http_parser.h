#ifndef HTTP_PARSER_H_INCLUDED
#define HTTP_PARSER_H_INCLUDED

#define HEADER_SIZE_LIMIT 4096
#define PATH_SIZE_LIMIT 256
#define METHOD_SIZE_LIMIT 16
#define HTTP_VERSION_SIZE_LIMIT 16


#define STOP_ERROR(req) { req->state = STATE_ERROR; return 0; }

typedef enum {
    STATE_ERROR,
    STATE_PROCESSING_REQUEST_LINE,
    STATE_PROCESSING_HEADER,
    STATE_HEADER_DONE
} http_parse_request_state;

struct http_param_list{
    char *key;
    char *value;
    struct http_param_list *next;
};
typedef struct http_param_list http_param_list;

struct http_parse_request{
    http_parse_request_state state;
    int seq_empty_line_count;
    char header[HEADER_SIZE_LIMIT + 1];
    char *header_curr;
    char *header_to;

    http_param_list *params;
    char http_version[HTTP_VERSION_SIZE_LIMIT + 1];
    char method[METHOD_SIZE_LIMIT + 1];
    char path[PATH_SIZE_LIMIT + 1];
};
typedef struct http_parse_request http_parse_request;

http_parse_request* http_parse_create_request();
int http_proceed_request(http_parse_request *request, char *newbuf, int newBufSize);
char* http_parser_find_param(http_parse_request *request, char *param_name);
void http_parse_free_request(http_parse_request *request);

#endif // HTTP_PARSER_H_INCLUDED
