// This file helps our server understand HTTP requests and build HTTP responses
#include "http.h"

#include <stdio.h>  
#include <string.h>  
#include <stddef.h>  

int http_parse_request_line(const char *data, size_t length, HttpRequest *request) {
    // Validate input parameters
    if (!data || length == 0 || !request) {
        return -1;
    }

    // Find the end of the method
    const char *method_end = memchr(data, ' ', length);
    if (!method_end) {
        return -1;
    }

    // Copy the method
    size_t method_len = method_end - data;
    if (method_len == 0 || method_len >= HTTP_METHOD_MAX) {
        return -1;
    }
    memcpy(request->method, data, method_len);
    request->method[method_len] = '\0';

    // Find the end of the path
    const char *path_start = method_end + 1;
    size_t remaining = length - (size_t)(path_start - data);

    const char *path_end = memchr(path_start, ' ', remaining);
    if (!path_end) {
        return -1;
    }

    // Copy the path
    size_t path_len = path_end - path_start;
    if (path_len == 0 || path_len >= HTTP_PATH_MAX) {
        return -1;
    }
    memcpy(request->path, path_start, path_len);
    request->path[path_len] = '\0';

    // Find the end of the version
    const char *version_start = path_end + 1;
    remaining = length - (size_t)(version_start - data);

    const char *version_end = memchr(version_start, '\r', remaining);
    if (!version_end) {
        return -1;
    }

    // Make sure the request line ends with \r\n
    if ((size_t)(version_end - data) + 1 >= length || version_end[1] != '\n') {
        return -1;
    }

    // Copy the version
    size_t version_len = version_end - version_start;
    if (version_len == 0 || version_len >= HTTP_VERSION_MAX) {
        return -1;
    }
    memcpy(request->version, version_start, version_len);
    request->version[version_len] = '\0';

    return 0;
}


HttpStatus http_get_status(const HttpRequest *request) {
    if (!request) {
        return HTTP_BAD_REQUEST;
    }

    // Check for supported HTTP versions
    if (strcmp(request->version, "HTTP/1.1") != 0 &&
        strcmp(request->version, "HTTP/1.0") != 0) {
        return HTTP_BAD_REQUEST;
    }

    // Check for supported HTTP methods
    if (strcmp(request->method, "GET") != 0) {
        return HTTP_METHOD_NOT_ALLOWED;
    }

    // Check if the requested path exists
    if (strcmp(request->path, "/") != 0) {
        return HTTP_NOT_FOUND;
    }

    return HTTP_OK;
}

int http_build_response(HttpStatus status, char *buffer, size_t buffer_size) {
    const char *status_text;
    const char *body;
    if (!buffer || buffer_size == 0) {
        return -1;
    }

    switch (status) {
        case HTTP_OK:
            status_text = "OK";
            body = "Hello, World!";
            break;
        case HTTP_BAD_REQUEST:
            status_text = "Bad Request";
            body = "400 Bad Request";
            break;
        case HTTP_NOT_FOUND:
            status_text = "Not Found";
            body = "404 Not Found";
            break;
        case HTTP_METHOD_NOT_ALLOWED:
            status_text = "Method Not Allowed";
            body = "405 Method Not Allowed";
            break;
        default:
            return -1; // Unsupported status code
    }

    // Build the HTTP response
    int written = snprintf(buffer, buffer_size,
                           "HTTP/1.1 %d %s\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: %zu\r\n"
                           "Connection: close\r\n"
                           "\r\n"
                           "%s",
                           status, status_text, strlen(body), body);

    // Check if snprintf was successful and if the buffer was large enough
    if (written < 0 || (size_t)written >= buffer_size) {
        return -1; // Error or buffer too small
    }

    return written; // Return the number of bytes written
}

