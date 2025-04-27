// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 2048

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    while (1) {
        // Clear buffer
        memset(buffer, 0, sizeof(buffer));

        // 1. Read command from user
        printf("Enter command (or QUIT): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Input error.\n");
            break;
        }

        // Remove newline from fgets
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "QUIT") == 0) {
            break;
        }

        // 2. Send command
        send(sock, buffer, strlen(buffer), 0);

        // 3. Check if command was WRITE (special case)
        if (strncmp(buffer, "WRITE", 5) == 0) {
            // User input format: WRITE filename offset length
            char filename[256];
            int offset, length;
            sscanf(buffer, "WRITE %s %d %d", filename, &offset, &length);

            printf("Enter %d bytes of data to write:\n", length);
            char *data = malloc(length);
            if (!data) {
                printf("Memory allocation error.\n");
                continue;
            }

            int bytes_read = read(STDIN_FILENO, data, length); // read raw bytes
            send(sock, data, bytes_read, 0);
            free(data);
        }

        // 4. Receive server reply
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        buffer[bytes] = '\0'; // null-terminate
        printf("Server reply: %s\n", buffer);
    }

    close(sock);
    return 0;
}
