#include "server.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

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

        // store incoming bytes
        char buffer[1024];

        // receive data from the client and reserve space for a null terminator
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received == -1) {
            perror("recv failed");
            close(client_fd);
            continue; // continue to the next iteration of the loop
        }

        if (bytes_received == 0) {
            printf("Client closed the connection\n");
            close(client_fd);
            continue; // continue to the next iteration of the loop
        }

        // null-terminate the received data
        buffer[bytes_received] = '\0';

        // print the received data to the console
        printf("Received data:\n%s\n", buffer);

        // response to send back to the client
        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "Hello, World!";

        // send the response to the client
        ssize_t bytes_sent = send(client_fd, response, strlen(response), 0);

        if (bytes_sent == -1) {
            perror("send failed");
        } else {
            printf("Sent %zd bytes to client\n", bytes_sent);
        }

        // close the client socket after handling the connection
        close(client_fd);
    }

    // close the server socket when done
    close(server_fd);
    return 0;
}