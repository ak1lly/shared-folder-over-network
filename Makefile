# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O2
CONFIGS = `pkg-config fuse3 --cflags --libs` 

# Targets
all: server client

server: server.c
	$(CC) $(CFLAGS) server.c -o server

client: client.c
	$(CC) $(CFLAGS) client.c -o client

networkFS: networkFS.c
	$(CC) $(CFLAGS) networkFS.c -o networkFS $(CONFIGS)

# Clean build artifacts
clean:
	rm -f server client networkFS
