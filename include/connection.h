#ifndef CONNECTION_H
#define CONNECTION_H

#include <stddef.h>

#define CONNECTION_BUFFER_SIZE 8192

typedef enum {
    CONNECTION_READING,
    CONNECTION_WRITING,
    CONNECTION_CLOSED
} ConnectionState;

typedef struct {
    int fd;

    ConnectionState state;

    char input_buffer[CONNECTION_BUFFER_SIZE];
    size_t input_length;

    char output_buffer[CONNECTION_BUFFER_SIZE];
    size_t output_length;
    size_t output_sent;
} Connection;

void connection_init(Connection *connection, int fd);
void connection_reset(Connection *connection);

#endif