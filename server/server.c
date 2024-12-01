#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "encryption.h" // Include the header for encryption functions

#pragma comment(lib, "ws2_32.lib") // Link against Winsock library

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024
#define DELIMITER "\n" // To mark the end of each message
#define ENCRYPTION_KEY "mysecurekey" // Encryption key

void start_server() {
    WSADATA wsa_data;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr); // Winsock uses `int` instead of `socklen_t`
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("[SERVER ERROR] Winsock initialization failed. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    // Create the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("[SERVER ERROR] Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("[SERVER ERROR] Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("[SERVER ERROR] Listen failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Listening on port %d...\n", SERVER_PORT);

    while (1) {
        printf("[SERVER] Waiting for a new client...\n");

        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("[SERVER ERROR] Client connection failed. Error Code: %d\n", WSAGetLastError());
            continue;
        }

        printf("[SERVER] Client connected.\n");

        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
            if (bytes_received <= 0) {
                printf("[SERVER] Client disconnected.\n");
                break;
            }

            // Decrypt the received message
            xor_decrypt(buffer, bytes_received, ENCRYPTION_KEY, strlen(ENCRYPTION_KEY));
            buffer[bytes_received] = '\0'; // Null-terminate
            printf("[SERVER RECEIVED] %s\n", buffer);

            // Encrypt and send the response
            char response[BUFFER_SIZE];
            snprintf(response, BUFFER_SIZE, "Server received: %s%s", buffer, DELIMITER);
            xor_encrypt_decrypt(response, strlen(response), ENCRYPTION_KEY, strlen(ENCRYPTION_KEY));
            send(client_socket, response, strlen(response), 0);
        }

        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    printf("[SERVER] Server closed.\n");
}

int main() {
    start_server();
    return 0;
}
