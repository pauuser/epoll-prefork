#ifndef HTTP_WRITER_H_INCLUDED
#define HTTP_WRITER_H_INCLUDED

void http_write_status_line(int fd, const char *http_version, const char *status_code, const char *error_phrase);
void http_write_header(int fd, const char *key, const char *value);
void http_write_int_header(int fd, const char *key, int value);
void http_empty_line(int fd);

#define STATUS_200(fd, http_version) http_write_status_line(fd, http_version, "200", "OK")
#define STATUS_400(fd, http_version) http_write_status_line(fd, http_version, "400", "Bad Request")
#define STATUS_403(fd, http_version) http_write_status_line(fd, http_version, "403", "Forbidden")
#define STATUS_404(fd, http_version) http_write_status_line(fd, http_version, "404", "Not Found")
#define STATUS_405(fd, http_version) http_write_status_line(fd, http_version, "405", "Method Not Allowed")
#define STATUS_406(fd, http_version) http_write_status_line(fd, http_version, "406", "Unacceptable")
#define STATUS_500(fd, http_version) http_write_status_line(fd, http_version, "500", "Internal Server Error")


#define PARAM_DATE(fd, value)              http_write_header(fd, "Date", value)
#define PARAM_SERVER(fd, value)            http_write_header(fd, "Server", value)
#define PARAM_CONTENT_LENGTH(fd, value)    http_write_int_header(fd, "Content-Length", value)
#define PARAM_CONTENT_TYPE(fd, value)      http_write_header(fd, "Content-Type", value)
#define PARAM_CONNECTION(fd, value)        http_write_header(fd, "Connection", value)


#endif // HTTP_WRITER_H_INCLUDED
