#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>    // for LS
#include <sys/stat.h>  // optional for later

#define PORT 8888
#define BUFFER_SIZE 2048

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    // 1. Setup server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) { perror("socket"); exit(EXIT_FAILURE); }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind"); exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) {
        perror("listen"); exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_fd < 0) { perror("accept"); continue; }

        printf("Client connected.\n");

        while (1) {
            memset(buffer, 0, sizeof(buffer));  // Clear buffer
            int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if (bytes_read <= 0) {
                printf("Client disconnected.\n");
                break;
            }

            printf("Received: %s\n", buffer);

            // 2. Parse command
            char command[10], arg1[256], arg2[256], arg3[256];
            memset(command, 0, sizeof(command));
            memset(arg1, 0, sizeof(arg1));
            memset(arg2, 0, sizeof(arg2));
            memset(arg3, 0, sizeof(arg3));
            
            sscanf(buffer, "%s %s %s %s", command, arg1, arg2, arg3);

            // 3. Dispatch command
            if (strcmp(command, "OPEN") == 0) {
                FILE *file = fopen(arg1, "r+");
                if (!file) file = fopen(arg1, "w+");
                if (file) {
                    fclose(file);
                    send(client_fd, "OK\n", 3, 0);
                } else {
                    send(client_fd, "ERR Cannot open file\n", 22, 0);
                }
            } 
            else if (strcmp(command, "READ") == 0) {
                FILE *file = fopen(arg1, "r");
                if (!file) {
                    send(client_fd, "ERR Cannot open file\n", 22, 0);
                } else {
                    int offset = atoi(arg2);
                    int length = atoi(arg3);
                    fseek(file, offset, SEEK_SET);

                    char data[1024];
                    int n = fread(data, 1, length, file);
                    fclose(file);

                    if (n > 0) {
                        send(client_fd, "OK ", 3, 0);
                        send(client_fd, data, n, 0);
                    } else {
                        send(client_fd, "ERR Cannot read\n", 16, 0);
                    }
                }
            } 
            else if (strcmp(command, "WRITE") == 0) {
                FILE *file = fopen(arg1, "r+");
                if (!file) {
                    send(client_fd, "ERR Cannot open file\n", 22, 0);
                } else {
                    int offset = atoi(arg2);
                    int length = atoi(arg3);

                    char *data = malloc(length);
                    if (!data) {
                        send(client_fd, "ERR Memory error\n", 17, 0);
                        fclose(file);
                        continue;
                    }
                    
                    // Need to read the <data> part (after command)
                    recv(client_fd, data, length, 0);

                    fseek(file, offset, SEEK_SET);
                    int n = fwrite(data, 1, length, file);
                    fclose(file);
                    free(data);

                    if (n == length) {
                        send(client_fd, "OK\n", 3, 0);
                    } else {
                        send(client_fd, "ERR Write error\n", 16, 0);
                    }
                }
            } 
            else if (strcmp(command, "LS") == 0) {
                DIR *d;
                struct dirent *dir;
                d = opendir(arg1);
                if (d) {
                    send(client_fd, "OK ", 3, 0);
                    while ((dir = readdir(d)) != NULL) {
                        send(client_fd, dir->d_name, strlen(dir->d_name), 0);
                        send(client_fd, " ", 1, 0);
                    }
                    closedir(d);
                } else {
                    send(client_fd, "ERR Cannot open directory\n", 26, 0);
                }
            } 
            else {
                send(client_fd, "ERR Unknown command\n", 21, 0);
            }
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
