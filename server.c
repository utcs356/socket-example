// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];

    free(arg);

    // Communicate with the client
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int read_size = read(client_socket, buffer, BUFFER_SIZE);
        if (read_size <= 0) {
            printf("Client disconnected\n");
            break;
        }
        printf("Received: %s\n", buffer);
        send(client_socket, "Hello from server", strlen("Hello from server"), 0);
    }

    close(client_socket);
    return NULL;
}

int main(void) {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Step 1: Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Step 2: Bind the socket to an address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Step 3: Listen for connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Step 4: Accept a connection
        int *client_socket = malloc(sizeof(int));
        if ((*client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            free(client_socket);
            continue;
        }

        printf("New client connected\n");

        // Create a thread to handle the client
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, client_socket) != 0) {
            perror("Failed to create thread");
            free(client_socket);
        }
        pthread_detach(thread_id);
    }

    close(server_fd);
    return 0;
}
