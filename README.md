---

# 🧭 Step-by-Step Roadmap: "Shared Folder over Network" in C

---

## Phase 1: Core Networking (TCP Server & Client)

✅ **Goal:**  
First, **ignore the filesystem**.  
Just **make a C server and client** that send simple messages like "list files", "read file", "write file".

---

### (1) Server

Write a C program that:
- Opens a TCP socket
- Binds to a port
- Listens for incoming connections
- Accepts a client
- Reads a command like `"LIST"`, `"READ filename"`, `"WRITE filename"`

Example pseudocode:
```c
bind(server_socket, port);
listen(server_socket);
while (1) {
    client = accept(server_socket);
    recv(client, command);
    if (strcmp(command, "LIST") == 0) {
        send back list of files;
    }
    else if (strcmp(command, "READ file.txt") == 0) {
        open file.txt;
        send file contents;
    }
    else if (strcmp(command, "WRITE file.txt") == 0) {
        receive data and save to file.txt;
    }
    close(client);
}
```

---

### (2) Client

Write a C program that:
- Connects to the server's IP and port
- Sends a command like `"LIST"`, `"READ file.txt"`, `"WRITE file.txt"`

Example pseudocode:
```c
connect(client_socket, server_ip, port);
send(client_socket, "READ file.txt");
recv(client_socket, data);
```

---

✅ Once you have this working, you **already have basic file sharing** (but only through custom CLI programs).

---

## Phase 2: Protocol Design

✅ **Goal:**  
Create a **simple custom protocol** to send file requests cleanly.

Define your protocol like:
```text
<COMMAND> <FILENAME> <BYTES> <DATA>

Examples:
LIST
READ filename.txt
WRITE filename.txt <4-bytes of size> <then that many bytes of data>
DELETE filename.txt
```
> (later you can make it more robust by adding headers, checksums, etc.)

---

## Phase 3: Basic Shared Folder

✅ **Goal:**  
Make the **server side** store the shared folder's files in a single directory (e.g., `/home/shared`).

In server code:
- When client says `READ file.txt`, you actually `open("/home/shared/file.txt")`
- When client says `WRITE file.txt`, you `write("/home/shared/file.txt")`
- When client says `LIST`, you read all files in `/home/shared`

This makes it truly a **shared folder** on the server!

---

## Phase 4: Filesystem Mount (Optional Now, Later)

✅ **Goal:**  
Make the client feel like it’s mounting the folder as a real filesystem.

You have 2 paths here:
| Option | How | Difficulty |
|:-------|:---|:-----------|
| FUSE (Filesystem in Userspace) | Use `libfuse` to mount remote shared folder | Moderate |
| Full Kernel VFS module | Implement mount and superblock in kernel module | Hardcore |

**FUSE path** is way more practical for now.

---

## Phase 5: Nice-to-Have Features

✅ After basic file sharing works, you can add:
- Authentication (require password to connect)
- Encryption (e.g., simple TLS)
- Version control (detect file updates)
- Locking (to prevent two clients from overwriting same file)
- File caching
- Compression for network efficiency

---

# 📜 Overall Roadmap Summary

| Step | What to Build | Why |
|:----|:--------------|:---|
| 1 | TCP server and TCP client in C | Basic communication |
| 2 | Define simple command protocol | Clean communication |
| 3 | Server: handle LIST/READ/WRITE in shared folder | Real "shared folder" |
| 4 | Client: CLI to list/read/write remotely | Usable minimal system |
| 5 | (Later) Build a FUSE client | Full local mount support |

---

# 🛠 Things You Need to Know in C

You’ll need to learn or use:
- `socket()`, `bind()`, `listen()`, `accept()`, `connect()`
- `send()`, `recv()`
- `open()`, `read()`, `write()` for files
- `opendir()`, `readdir()`, `closedir()` to list files in a folder
- `fork()`, `select()`, or `epoll()` for handling multiple clients (later)

---


# 📦 Project Sketch

```
sharedfs/
├── server/
│   ├── sharedfs_server.c
│   └── Makefile
├── client/
│   ├── sharedfs_client.c
│   └── Makefile
├── README.md
└── protocol.txt
```

