#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024
#define DELIMITER "\n" // To mark the end of each message

void start_server() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("[SERVER ERROR] Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[SERVER ERROR] Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(server_socket, 5) < 0) {
        perror("[SERVER ERROR] Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Listening on port %d...\n", SERVER_PORT);

    // Accept a client connection
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_socket < 0) {
        perror("[SERVER ERROR] Client connection failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Client connected.\n");

    // Communication loop
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("[SERVER] Client disconnected.\n");
            break;
        }

        // Ensure buffer ends with a null terminator
        buffer[bytes_received] = '\0';
        printf("[SERVER RECEIVED] %s", buffer);

        // Respond to the client
        char response[BUFFER_SIZE];
        snprintf(response, BUFFER_SIZE, "Server received: %s%s", buffer, DELIMITER);
        send(client_socket, response, strlen(response), 0);
    }

    // Close sockets
    close(client_socket);
    close(server_socket);
    printf("[SERVER] Server closed.\n");
}

int main() {
    start_server();
    return 0;
}
