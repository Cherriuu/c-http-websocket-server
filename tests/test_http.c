#include "http.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(void) {
    const char *data = "GET /hello HTTP/1.1\r\n";
    HttpRequest request = {0};

    int result = http_parse_request_line(data, strlen(data), &request);

    assert(result == 0);
    assert(strcmp(request.method, "GET") == 0);
    assert(strcmp(request.path, "/hello") == 0);
    assert(strcmp(request.version, "HTTP/1.1") == 0);

    printf("Test passed: valid GET request\n");

    return 0;
}