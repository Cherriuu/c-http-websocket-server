#include "connection.h"

void connection_init(Connection *c, int fd)
{
    c->fd = fd;
    c->state = CONN_READING;

    c->input_length = 0;
    c->output_length = 0;
    c->output_sent = 0;

    c->keep_alive = 0;
}

void connection_reset(Connection *c)
{
    c->state = CONN_READING;

    c->input_length = 0;
    c->output_length = 0;
    c->output_sent = 0;

    c->keep_alive = 0;
}