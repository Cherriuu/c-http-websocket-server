#ifndef HTTP_H
#define HTTP_H

#include "connection.h"

int http_headers_complete(const char *buffer, size_t length);
int http_build_response(Connection *c);

#endif