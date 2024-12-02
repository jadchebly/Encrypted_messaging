#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "encryption.h"

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

#define PORT 12345
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    SOCKET socket;
    char username[50];
} Client;

Client clients[MAX_CLIENTS];
CRITICAL_SECTION clients_mutex; // Mutex for thread safety

void broadcast_message(const char *message, SOCKET exclude_socket) {
    EnterCriticalSection(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket != 0 && clients[i].socket != exclude_socket) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }
    LeaveCriticalSection(&clients_mutex);
}

void send_private_message(SOCKET sender_socket, const char *sender_username, const char *message) {
    char recipient[50];
    char private_message[BUFFER_SIZE];
    sscanf(message, "/pm %s %[^\n]", recipient, private_message);

    EnterCriticalSection(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket != 0 && strcmp(clients[i].username, recipient) == 0) {
            char formatted_message[BUFFER_SIZE];
            snprintf(formatted_message, sizeof(formatted_message), "[Private from %s]: %s\n", sender_username, private_message);
            send(clients[i].socket, formatted_message, strlen(formatted_message), 0);
            LeaveCriticalSection(&clients_mutex);
            return;
        }
    }
    LeaveCriticalSection(&clients_mutex);

    // Notify sender if recipient is not found
    char error_message[BUFFER_SIZE];
    snprintf(error_message, sizeof(error_message), "User %s not found\n", recipient);
    send(sender_socket, error_message, strlen(error_message), 0);
}

DWORD WINAPI handle_client(LPVOID arg) {
    SOCKET client_socket = *((SOCKET *)arg);
    free(arg);

    char buffer[BUFFER_SIZE];
    char username[50];
    recv(client_socket, username, sizeof(username), 0);

    EnterCriticalSection(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket == 0) {
            clients[i].socket = client_socket;
            strncpy(clients[i].username, username, sizeof(clients[i].username));
            break;
        }
    }
    LeaveCriticalSection(&clients_mutex);

    char welcome_message[BUFFER_SIZE];
    snprintf(welcome_message, sizeof(welcome_message), "%s has joined the chat\n", username);
    printf("%s", welcome_message);
    broadcast_message(welcome_message, client_socket);

    while (1) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break;
        }
        buffer[bytes_received] = '\0';

        if (strncmp(buffer, "/pm", 3) == 0) {
            send_private_message(client_socket, username, buffer);
        } else {
            char formatted_message[BUFFER_SIZE];
            snprintf(formatted_message, sizeof(formatted_message), "[%s]: %s", username, buffer);
            printf("%s", formatted_message);
            broadcast_message(formatted_message, client_socket);
        }
    }

    EnterCriticalSection(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket == client_socket) {
            clients[i].socket = 0;
            break;
        }
    }
    LeaveCriticalSection(&clients_mutex);

    snprintf(welcome_message, sizeof(welcome_message), "%s has left the chat\n", username);
    printf("%s", welcome_message);
    broadcast_message(welcome_message, client_socket);
    closesocket(client_socket);
    return 0;
}

int main() {
    WSADATA wsa_data;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("Winsock initialization failed\n");
        exit(1);
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        exit(1);
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(server_socket);
        WSACleanup();
        exit(1);
    }

    if (listen(server_socket, MAX_CLIENTS) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server_socket);
        WSACleanup();
        exit(1);
    }

    printf("Server started on port %d\n", PORT);
    InitializeCriticalSection(&clients_mutex);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed\n");
            continue;
        }

        SOCKET *new_sock = malloc(sizeof(SOCKET));
        *new_sock = client_socket;

        HANDLE thread = CreateThread(NULL, 0, handle_client, new_sock, 0, NULL);
        if (thread == NULL) {
            printf("Thread creation failed\n");
            closesocket(client_socket);
        }
    }

    DeleteCriticalSection(&clients_mutex);
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
