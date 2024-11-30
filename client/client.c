#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link against Winsock library

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024
#define DELIMITER "\n" // To mark the end of each message

void start_client() {
    WSADATA wsa_data;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("[CLIENT ERROR] Winsock initialization failed. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    // Create the socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("[CLIENT ERROR] Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("[CLIENT ERROR] Connection to server failed. Error Code: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
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

    // Close socket and cleanup Winsock
    closesocket(client_socket);
    WSACleanup();
    printf("[CLIENT] Connection closed.\n");
}

int main() {
    start_client();
    return 0;
}
