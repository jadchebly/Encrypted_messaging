#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    char username[50];
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast_message(const char *message, int exclude_socket) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket != 0 && clients[i].socket != exclude_socket) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_private_message(int sender_socket, const char *sender_username, const char *message) {
    char recipient[50];
    char private_message[BUFFER_SIZE];
    sscanf(message, "/pm %s %[^\n]", recipient, private_message);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket != 0 && strcmp(clients[i].username, recipient) == 0) {
            char formatted_message[BUFFER_SIZE];
            snprintf(formatted_message, sizeof(formatted_message), "[Private from %s]: %s\n", sender_username, private_message);
            send(clients[i].socket, formatted_message, strlen(formatted_message), 0);
            pthread_mutex_unlock(&clients_mutex);
            return;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    // If recipient is not found, notify the sender
    char error_message[BUFFER_SIZE];
    snprintf(error_message, sizeof(error_message), "User %s not found\n", recipient);
    send(sender_socket, error_message, strlen(error_message), 0);
}

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    free(arg);

    char buffer[BUFFER_SIZE];
    char username[50];
    recv(client_socket, username, sizeof(username), 0);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket == 0) {
            clients[i].socket = client_socket;
            strncpy(clients[i].username, username, sizeof(clients[i].username));
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    char welcome_message[BUFFER_SIZE];
    snprintf(welcome_message, sizeof(welcome_message), "%s has joined the chat\n", username);
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
            broadcast_message(formatted_message, client_socket);
        }
    }

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket == client_socket) {
            clients[i].socket = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    snprintf(welcome_message, sizeof(welcome_message), "%s has left the chat\n", username);
    broadcast_message(welcome_message, client_socket);
    close(client_socket);
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in server_address;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(1);
    }

    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server started on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        int *new_sock = malloc(sizeof(int));
        *new_sock = client_socket;

        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, (void *)new_sock);
        pthread_detach(thread);
    }

    close(server_socket);
    return 0;
}