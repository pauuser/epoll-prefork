#include "http_parser.h"
#include "../../utils/utils.h"
#include <string.h>
#include <sys/param.h>

http_parse_request* http_parse_create_request(){
    http_parse_request *item = (http_parse_request *)malloc(sizeof(http_parse_request));
    item->header_curr = item->header;
    item->header_to = item->header;
    item->seq_empty_line_count = 0;
    item->params = NULL;
    item->state = STATE_PROCESSING_REQUEST_LINE;
    return item;
}

/*
Process yet another block of input socket data.
newbuf - block of data of newBufSize size

returns the number of used newbuf characters (unused data corresponds to message body)
*/
int http_proceed_request(http_parse_request *request, char *newbuf, int newBufSize){
    if (request->state != STATE_PROCESSING_REQUEST_LINE &&
        request->state != STATE_PROCESSING_HEADER){
        return 0;
    }

    char *head_end = request->header + HEADER_SIZE_LIMIT - 1,
            *orig_to = request->header_to;

    // copy input data to request header buffer
    while (request->header_to < head_end && newBufSize-- > 0){
        *(request->header_to)++ = *newbuf++;
    }
    *(request->header_to) = '\0';

    char *line_end = request->header_curr, *last_line_end = orig_to;
    while (line_end + 1 <= head_end && *line_end != '\0'){
        if (*(line_end) != '\r' || *(line_end + 1) != '\n'){
            line_end++;
            continue;
        }
        // Now we have a complete line in request->header to be processed

        last_line_end = line_end + 1;
        int line_size = line_end - request->header_curr - 1;
        if (line_size <= 0){
            // Processing empty line case
            if (request->state == STATE_PROCESSING_HEADER){
                // end of header
                request->state = STATE_HEADER_DONE;
                return max(line_end + 1 - orig_to, 0);
            }
            else{
                STOP_ERROR(request);
            }
            request->header_curr = line_end + 1;
            line_end += 2;
        }
        else{
            // Processing non-empty line
            if (request->state == STATE_PROCESSING_REQUEST_LINE){
                if(http_query_request(request->header_curr, line_end, request)){
                    request->state = STATE_PROCESSING_HEADER;
                }else{
                    STOP_ERROR(request);
                }
            }else if (request->state == STATE_PROCESSING_HEADER
                && !http_query_keyvalue(request->header_curr, line_end, request)){
                STOP_ERROR(request);
            }
            request->header_curr = line_end + 1;
            line_end += 2;
        }
    }

    return max(0, last_line_end - orig_to);
}

int http_query_request(char *start, char *end, http_parse_request *request){
    start = ltrim(start);
    start = copy_till(start, request->method, ' ', METHOD_SIZE_LIMIT);
    if (!start || ! (*start)){
        return 0;
    }
    start = ltrim(start);

    start = copy_till(start, request->path, ' ', PATH_SIZE_LIMIT);
    if (!start || ! (*start)){
        return 0;
    }

    start = ltrim(start);
    start = copy_till(start, request->http_version, '\r', HTTP_VERSION_SIZE_LIMIT);
    if (!start){
        return 0;
    }

    return 1;
}

int http_query_keyvalue(char *start, char *end, http_parse_request *request){
    start = ltrim(start);
    char *key_end = strstr(start, ":"), *key, *value;
    if (!key_end || key_end > end){
        return 0;
    }

    int key_len = key_end - start ;
    key = (char *)malloc(sizeof(char)*(key_len + 1));
    memcpy(key, start, key_len);
    key[key_len] = '\0';
    key = rtrim(key);

    start = key_end + 1;
    start = ltrim(start);
    while(start < end && is_wspace(start)) start++;

    int valueLen = end - start;
    if (valueLen <= 0){
        value = strdup("");
    }else{
        value = (char *)malloc(sizeof(char)*(valueLen+1));
        memcpy(value, start, valueLen);
        value[valueLen] = '\0';
    }

    http_param_list *listItem = (http_param_list *)malloc(sizeof(http_param_list));
    listItem->key = key;
    listItem->value = value;
    listItem->next = request->params;
    request->params = listItem;

    return 1;
}

char * http_parser_find_param(http_parse_request *request, char *param_name){
    http_param_list *curr = request->params;
    while (curr != NULL){
        if (stricmp(curr->key, param_name) == 0){
            return curr->value;
        }
        curr = curr->next;
    }
    return NULL;
}

void http_parse_free_request(http_parse_request *request){
    http_param_list *curr_param = request->params;
    while (curr_param != NULL){
        http_param_list *next = curr_param->next;
        free(curr_param->key);
        free(curr_param->value);
        free(curr_param);
        curr_param = next;
    }
    free(request);
}
