#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define REQUEST_BUFFER_SIZE 8192
#define FILE_BUFFER_SIZE 4096
#define PATH_BUFFER_SIZE 512

static void send_all(int client_fd, const char *data, size_t length)
{
    size_t total_sent = 0;

    while (total_sent < length) {
        ssize_t sent = send(
            client_fd,
            data + total_sent,
            length - total_sent,
            0
        );

        if (sent <= 0) {
            return;
        }

        total_sent += (size_t)sent;
    }
}

static const char *get_content_type(const char *path)
{
    const char *extension = strrchr(path, '.');

    if (extension == NULL) {
        return "application/octet-stream";
    }

    if (strcmp(extension, ".html") == 0) {
        return "text/html";
    }

    if (strcmp(extension, ".css") == 0) {
        return "text/css";
    }

    if (strcmp(extension, ".js") == 0) {
        return "application/javascript";
    }

    if (strcmp(extension, ".json") == 0) {
        return "application/json";
    }

    if (strcmp(extension, ".png") == 0) {
        return "image/png";
    }

    if (strcmp(extension, ".jpg") == 0 ||
        strcmp(extension, ".jpeg") == 0) {
        return "image/jpeg";
    }

    if (strcmp(extension, ".txt") == 0) {
        return "text/plain";
    }

    return "application/octet-stream";
}

static void send_error_response(
    int client_fd,
    int status_code,
    const char *status_text,
    const char *message
)
{
    char body[512];

    int body_length = snprintf(
        body,
        sizeof(body),
        "<!DOCTYPE html>"
        "<html>"
        "<head><title>%d %s</title></head>"
        "<body>"
        "<h1>%d %s</h1>"
        "<p>%s</p>"
        "</body>"
        "</html>",
        status_code,
        status_text,
        status_code,
        status_text,
        message
    );

    char response[1024];

    int response_length = snprintf(
        response,
        sizeof(response),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        status_code,
        status_text,
        body_length,
        body
    );

    send_all(
        client_fd,
        response,
        (size_t)response_length
    );
}

static int path_is_safe(const char *path)
{
    if (strstr(path, "..") != NULL) {
        return 0;
    }

    return 1;
}

static void serve_file(
    int client_fd,
    const char *request_path
)
{
    char file_path[PATH_BUFFER_SIZE];

    if (strcmp(request_path, "/") == 0) {
        snprintf(
            file_path,
            sizeof(file_path),
            "public/index.html"
        );
    } else {
        snprintf(
            file_path,
            sizeof(file_path),
            "public%s",
            request_path
        );
    }

    if (!path_is_safe(file_path)) {
        send_error_response(
            client_fd,
            403,
            "Forbidden",
            "Access to this path is not allowed."
        );

        return;
    }

    FILE *file = fopen(file_path, "rb");

    if (file == NULL) {
        send_error_response(
            client_fd,
            404,
            "Not Found",
            "The requested file could not be found."
        );

        return;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);

        send_error_response(
            client_fd,
            500,
            "Internal Server Error",
            "Failed to inspect requested file."
        );

        return;
    }

    long file_size = ftell(file);

    if (file_size < 0) {
        fclose(file);

        send_error_response(
            client_fd,
            500,
            "Internal Server Error",
            "Failed to determine file size."
        );

        return;
    }

    rewind(file);

    const char *content_type =
        get_content_type(file_path);

    char header[1024];

    int header_length = snprintf(
        header,
        sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n"
        "\r\n",
        content_type,
        file_size
    );

    send_all(
        client_fd,
        header,
        (size_t)header_length
    );

    char buffer[FILE_BUFFER_SIZE];

    size_t bytes_read;

    while ((bytes_read =
                fread(
                    buffer,
                    1,
                    sizeof(buffer),
                    file
                )) > 0) {

        send_all(
            client_fd,
            buffer,
            bytes_read
        );
    }

    fclose(file);
}

void handle_http_request(int client_fd)
{
    char request[REQUEST_BUFFER_SIZE];

    ssize_t bytes_received =
        recv(
            client_fd,
            request,
            sizeof(request) - 1,
            0
        );

    if (bytes_received <= 0) {
        return;
    }

    request[bytes_received] = '\0';

    char method[16];
    char path[PATH_BUFFER_SIZE];
    char version[16];

    int parsed = sscanf(
        request,
        "%15s %511s %15s",
        method,
        path,
        version
    );

    if (parsed != 3) {
        send_error_response(
            client_fd,
            400,
            "Bad Request",
            "The HTTP request could not be parsed."
        );

        return;
    }

    printf(
        "%s %s %s\n",
        method,
        path,
        version
    );

    if (strcmp(method, "GET") != 0) {
        send_error_response(
            client_fd,
            405,
            "Method Not Allowed",
            "Only GET requests are currently supported."
        );

        return;
    }

    if (strcmp(version, "HTTP/1.1") != 0 &&
        strcmp(version, "HTTP/1.0") != 0) {

        send_error_response(
            client_fd,
            505,
            "HTTP Version Not Supported",
            "Unsupported HTTP version."
        );

        return;
    }

    serve_file(client_fd, path);
}