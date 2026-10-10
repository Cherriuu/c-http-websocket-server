#include "connection.h"

#include <string.h>

void connection_init(Connection *connection, int fd)
{
    connection->fd = fd;
    connection->state = CONNECTION_READING;

    connection->input_length = 0;
    connection->output_length = 0;
    connection->output_sent = 0;

    memset(
        connection->input_buffer,
        0,
        sizeof(connection->input_buffer)
    );

    memset(
        connection->output_buffer,
        0,
        sizeof(connection->output_buffer)
    );
}

void connection_reset(Connection *connection)
{
    connection->state = CONNECTION_READING;

    connection->input_length = 0;
    connection->output_length = 0;
    connection->output_sent = 0;

    memset(
        connection->input_buffer,
        0,
        sizeof(connection->input_buffer)
    );

    memset(
        connection->output_buffer,
        0,
        sizeof(connection->output_buffer)
    );
}