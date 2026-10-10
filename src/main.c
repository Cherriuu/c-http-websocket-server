#include <stdio.h>
#include "server.h"

int main(void)
{
    printf("Starting C HTTP server...\n");

    if (server_start() == -1) {
        fprintf(stderr, "Server failed.\n");
        return 1;
    }

    return 0;
}