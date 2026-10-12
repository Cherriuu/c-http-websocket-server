// store the parsed information from an HTTP request

#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

// maximum lengths for HTTP request fields
#define HTTP_METHOD_MAX 16
#define HTTP_PATH_MAX 1024
#define HTTP_VERSION_MAX 16

// stores the parsed information from an HTTP request
typedef struct {
    char method[HTTP_METHOD_MAX];
    char path[HTTP_PATH_MAX];
    char version[HTTP_VERSION_MAX];
} HttpRequest;

typedef enum {
    HTTP_OK = 200,
    HTTP_BAD_REQUEST = 400,
    HTTP_NOT_FOUND = 404,
    HTTP_METHOD_NOT_ALLOWED = 405
} HttpStatus;

// parses an HTTP request line into an HttpRequest structure
// returns 0 on success, -1 on failure
int http_parse_request_line(const char *data, size_t length, HttpRequest *request);
int http_build_response(HttpStatus status, char *buffer, size_t buffer_size);

HttpStatus http_get_status(const HttpRequest *request);

#endif