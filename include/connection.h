#ifndef CONNECTION_H
#define CONNECTION_H

#include <stddef.h>

#define INPUT_BUFFER_SIZE 8192
#define OUTPUT_BUFFER_SIZE 65536

typedef enum {
    CONN_READING,
    CONN_WRITING,
    CONN_CLOSED
} ConnectionState;

typedef struct {
    int fd;
    ConnectionState state;

    char input[INPUT_BUFFER_SIZE];
    size_t input_length;

    char output[OUTPUT_BUFFER_SIZE];
    size_t output_length;
    size_t output_sent;

    int keep_alive;
} Connection;

void connection_init(Connection *c, int fd);
void connection_reset(Connection *c);

#endif