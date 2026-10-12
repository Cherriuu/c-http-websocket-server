#include "http.h"

#include <stdio.h>
#include <string.h>

#define PATH_SIZE 512

int http_headers_complete(const char *buffer, size_t length)
{
    for (size_t i = 0; i + 3 < length; i++) {
        if (buffer[i] == '\r' &&
            buffer[i + 1] == '\n' &&
            buffer[i + 2] == '\r' &&
            buffer[i + 3] == '\n') {
            return 1;
        }
    }

    return 0;
}

static const char *content_type(const char *path)
{
    const char *ext = strrchr(path, '.');

    if (!ext) return "application/octet-stream";
    if (!strcmp(ext, ".html")) return "text/html; charset=utf-8";
    if (!strcmp(ext, ".css")) return "text/css; charset=utf-8";
    if (!strcmp(ext, ".js")) return "application/javascript; charset=utf-8";
    if (!strcmp(ext, ".json")) return "application/json; charset=utf-8";
    if (!strcmp(ext, ".png")) return "image/png";
    if (!strcmp(ext, ".jpg") || !strcmp(ext, ".jpeg")) return "image/jpeg";
    if (!strcmp(ext, ".txt")) return "text/plain; charset=utf-8";

    return "application/octet-stream";
}

static int write_response(
    Connection *c,
    int status,
    const char *reason,
    const char *type,
    const void *body,
    size_t body_length
)
{
    int header_length = snprintf(
        c->output,
        sizeof(c->output),

        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: %s\r\n"
        "\r\n",

        status,
        reason,
        type,
        body_length,
        c->keep_alive ? "keep-alive" : "close"
    );

    if (header_length < 0 ||
        (size_t)header_length >= sizeof(c->output)) {
        return -1;
    }

    if (body_length >
        sizeof(c->output) - (size_t)header_length) {
        return -1;
    }

    memcpy(
        c->output + header_length,
        body,
        body_length
    );

    c->output_length =
        (size_t)header_length + body_length;

    c->output_sent = 0;
    c->state = CONN_WRITING;

    return 0;
}

static int error_response(
    Connection *c,
    int status,
    const char *reason,
    const char *message
)
{
    char body[512];

    int length = snprintf(
        body,
        sizeof(body),

        "<!doctype html>"
        "<h1>%d %s</h1>"
        "<p>%s</p>",

        status,
        reason,
        message
    );

    if (length < 0 ||
        (size_t)length >= sizeof(body)) {
        return -1;
    }

    return write_response(
        c,
        status,
        reason,
        "text/html; charset=utf-8",
        body,
        (size_t)length
    );
}

static int serve_file(
    Connection *c,
    const char *request_path
)
{
    if (request_path[0] != '/' ||
        strstr(request_path, "..")) {
        return error_response(
            c,
            403,
            "Forbidden",
            "Unsafe path."
        );
    }

    char path[PATH_SIZE];

    int n = snprintf(
        path,
        sizeof(path),

        "public%s",

        !strcmp(request_path, "/")
            ? "/index.html"
            : request_path
    );

    if (n < 0 ||
        (size_t)n >= sizeof(path)) {
        return error_response(
            c,
            414,
            "URI Too Long",
            "Path is too long."
        );
    }

    FILE *file = fopen(path, "rb");

    if (!file) {
        return error_response(
            c,
            404,
            "Not Found",
            "File not found."
        );
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);

        return error_response(
            c,
            500,
            "Internal Server Error",
            "Could not inspect file."
        );
    }

    long size = ftell(file);

    if (size < 0 ||
        (size_t)size > OUTPUT_BUFFER_SIZE - 1024) {
        fclose(file);

        return error_response(
            c,
            413,
            "Content Too Large",
            "File is too large."
        );
    }

    rewind(file);

    unsigned char body[OUTPUT_BUFFER_SIZE];

    size_t bytes_read =
        fread(body, 1, (size_t)size, file);

    fclose(file);

    if (bytes_read != (size_t)size) {
        return error_response(
            c,
            500,
            "Internal Server Error",
            "Could not read file."
        );
    }

    return write_response(
        c,
        200,
        "OK",
        content_type(path),
        body,
        bytes_read
    );
}

int http_build_response(Connection *c)
{
    if (!http_headers_complete(
            c->input,
            c->input_length)) {
        return 0;
    }

    char method[16];
    char path[PATH_SIZE];
    char version[16];

    if (sscanf(
            c->input,
            "%15s %511s %15s",
            method,
            path,
            version) != 3) {

        c->keep_alive = 0;

        return error_response(
            c,
            400,
            "Bad Request",
            "Could not parse request."
        );
    }

    if (strcmp(version, "HTTP/1.1") &&
        strcmp(version, "HTTP/1.0")) {

        c->keep_alive = 0;

        return error_response(
            c,
            505,
            "HTTP Version Not Supported",
            "Use HTTP/1.0 or HTTP/1.1."
        );
    }

    c->keep_alive =
        !strcmp(version, "HTTP/1.1");

    if (strstr(c->input, "Connection: close") ||
        strstr(c->input, "connection: close")) {
        c->keep_alive = 0;
    }

    if (strcmp(method, "GET")) {
        return error_response(
            c,
            405,
            "Method Not Allowed",
            "Only GET is supported."
        );
    }

    printf("%s %s %s\n", method, path, version);

    return serve_file(c, path);
}