#include "server.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// which port will the server listen on
#define PORT 8080
// how many pending connections can be queued
#define BACKLOG 128

int server_start(void) {
    // create the listening socket
    // the listening socket will be used to accept incoming connections
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        perror("socket allocation failed");
        return -1;
    }

    // used to store the address of the server
    struct sockaddr_in address = {0};
    // configure the address structure
    address.sin_family = AF_INET;
    // listen in on port 8080
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    // connect the socket to the specified address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind failed");
        close(server_fd);
        return -1;
    }

    // start listening for incoming connections
    if (listen(server_fd, BACKLOG) == -1) {
        perror("listen failed");
        close(server_fd);
        return -1;
    }
    else {
        printf("Server listening on port %d\n", PORT);
    }

    // server should keep accepting clients in an infinite loop
    while (1) {
        // accept a new client connection (ex: a web browser connecting to the server)
        int client_fd = accept(server_fd, NULL, NULL);

        if (client_fd == -1) {
            perror("accept failed");
            continue; // continue to the next iteration of the loop
        }
        else {
            printf("Accepted a connection\n");
        }

        receive_and_handle_client(client_fd);

        // close the client socket after handling the connection
        close(client_fd);
    }

    // close the server socket when done
    close(server_fd);
}