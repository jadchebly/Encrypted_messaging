#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define BUFFER_SIZE 1024



void *receive_messages(void *arg) {
    int socket = *((int *)arg);
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
    return NULL;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.59");

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(1);
    }

    char username[50];
    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';
    send(client_socket, username, strlen(username), 0);

    pthread_t thread;
    pthread_create(&thread, NULL, receive_messages, &client_socket);

    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket);
    return 0;
}