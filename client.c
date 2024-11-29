#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024
#define DELIMITER "\n" // To mark the end of each message

void start_client() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create the socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("[CLIENT ERROR] Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[CLIENT ERROR] Connection to server failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("[CLIENT] Connected to server at %s:%d\n", SERVER_IP, SERVER_PORT);

    // Communication loop
    while (1) {
        printf("[CLIENT] Enter a message (type 'exit' to quit): ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Remove newline character from input
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        // Check for exit condition
        if (strcmp(buffer, "exit") == 0) {
            printf("[CLIENT] Disconnecting.\n");
            break;
        }

        // Send message to server
        strcat(buffer, DELIMITER); // Add delimiter to mark message end
        send(client_socket, buffer, strlen(buffer), 0);

        // Receive server response
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("[CLIENT] Server disconnected.\n");
            break;
        }

        // Ensure buffer ends with a null terminator
        buffer[bytes_received] = '\0';
        printf("[CLIENT RECEIVED] %s", buffer);
    }

    // Close socket
    close(client_socket);
    printf("[CLIENT] Connection closed.\n");
}

int main() {
    start_client();
    return 0;
}
