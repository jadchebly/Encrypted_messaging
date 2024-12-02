#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "encryption.h"

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

#define PORT 12345
#define BUFFER_SIZE 1024

DWORD WINAPI receive_messages(LPVOID arg) {
    SOCKET socket = *((SOCKET *)arg);
    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes_received = recv(socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            printf("Disconnected from server\n");
            exit(1);
        }
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }
    return 0;
}

int main() {
    WSADATA wsa_data;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("Winsock initialization failed\n");
        exit(1);
    }

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        exit(1);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection failed\n");
        closesocket(client_socket);
        WSACleanup();
        exit(1);
    }

    char username[50];
    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';
    send(client_socket, username, strlen(username), 0);

    // Create thread to receive messages
    HANDLE thread = CreateThread(NULL, 0, receive_messages, &client_socket, 0, NULL);

    // Communication loop
    while (1) {
        printf("Enter message: ");
        fgets(buffer, sizeof(buffer), stdin);
        send(client_socket, buffer, strlen(buffer), 0);
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}
