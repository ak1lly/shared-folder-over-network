# 🗓 Week 1: TCP Client/Server + Protocol Design (Networking Foundation)

✅ Goal: Build your userspace networking side.

—

### 1. Write a **userspace TCP server** (in C)
- Listens on a port (e.g., 8888)
- Accepts one connection at a time
- Reads messages like: `"READ filename offset length"`
- Replies with dummy data: e.g., `"Hello World"`

Tip: Just use `socket()`, `bind()`, `listen()`, `accept()`, `recv()`, `send()` from `<sys/socket.h>`

—

### 2. Write a **userspace TCP client** (in C)
- Connects to your server
- Sends a dummy command ("PING")
- Receives a dummy reply ("PONG")

Get a basic roundtrip connection working first!

—

### 3. **Sketch your file operation protocol**
Design a simple "language" between client and server:
| Operation | Message Format Example |
|:----------|:------------------------|
| Open file | `"OPEN filename"` |
| Read file | `"READ filename offset length"` |
| Write file | `"WRITE filename offset length <data>"` |
| List directory | `"LS path"` |

You will extend this protocol later, but define the basics now.

—

### 4. **Networking Stress Test**
- Send lots of tiny reads
- Try dropping server connection
- See what happens if client reconnects

Focus on understanding TCP reliability.

---

# 🗓 Week 2: Basic FUSE Filesystem (Local only, No TCP yet)

✅ Goal: Create a fake filesystem in userspace using FUSE, no networking yet.

—

### 1. Install FUSE library
```bash
sudo apt install libfuse-dev
```

---

### 2. Write a **basic FUSE filesystem** (very simple)

Start with a tiny filesystem that:
- Always shows 1 fake file (`hello.txt`)
- `cat hello.txt` always prints `"Hello from FUSE!"`
- Hardcode dummy responses (no real server interaction yet)

Implement FUSE callbacks:
- `getattr` (stat a file)
- `readdir` (list directory)
- `open` (open file)
- `read` (read file contents)

FUSE Boilerplate structure:
```c
static struct fuse_operations myfs_oper = {
    .getattr = my_getattr,
    .readdir = my_readdir,
    .open = my_open,
    .read = my_read,
};
```

Then `fuse_main()` passes control to your operations.

---

### 3. Testing
Create Directory to mount on:
```bash
mkdir -p ~/mnt/myshare
```
Mount your FUSE filesystem:
```bash
./networkFS ~/mnt/myshare
```
Check:
```bash
$ ls ~/mnt/myshare
hello.txt
$ cat ~/mnt/myshare/hello.txt
Hello from FUSE!
```

Should work even though it’s all fake/hardcoded.

---

# 🗓 Week 3: Integrate Networking with FUSE (Real Server Interaction)

✅ Goal: Make your filesystem *talk* to the server over TCP!

—

### 1. Modify your FUSE read/write/open handlers

When `read()` is called:
- Connect to your server (or use a persistent socket)
- Send a `"READ filename offset length"` request
- Receive the real file content from server
- Return that content to user

When `readdir()` is called:
- Send `"LS /"` to server
- Receive list of filenames
- Dynamically return directory entries

When `open()` is called:
- Send `"OPEN filename"` to server
- Check if file exists or fail

—

### 2. Design Connection Strategy
You can:
- Keep a **persistent socket** open during filesystem mount
- OR connect/disconnect per operation (simpler but slower)

Pick one based on time and skill.

---

### 3. (Optional polish) Error Handling
- If server returns an error → return `-ENOENT` (file not found) to FUSE
- If server dies → unmount filesystem gracefully if possible

---

# 🚀 What You Should Have at the End

| Week | Milestone |
|:-----|:----------|
| 1 | Client/server talk over TCP |
| 2 | FUSE dummy filesystem mounted locally |
| 3 | FUSE filesystem fetching real data over network |

By the end of Week 3, you can:

✅ Mount `/mnt/myshare`  
✅ `ls` files that live on the server  
✅ `cat` files and actually pull data over the network  
✅ (Bonus) `echo hello > file.txt` writes remotely

---

# 🔥 Comparison to Full Kernel Module

| Feature | FUSE Userspace | Kernel Module |
|:--------|:---------------|:--------------|
| Development Speed | Much faster | Slow |
| Debugging | Easy (normal gdb/printfs) | Hard (dmesg, printk) |
| Risk | Zero (no crashes) | High (kernel panic) |
| Learning Depth | Medium (filesystem concepts) | Very High (kernel + VFS + sockets) |
| Realism (for OS development) | Moderate | Very high |

—

# 📜 Summary Table

| Week | Focus | Deliverable |
|:-----|:------|:------------|
| 1 | TCP Networking + Protocol | Basic server/client talking |
| 2 | Local FUSE FS | Mountable filesystem with fake data |
| 3 | TCP + FUSE Integration | Network-backed mountable filesystem |

—

# Final Tip:

✅ Start **small**.  
✅ Each week, finish something that actually "works", even if dumb.  
✅ Never design everything upfront — code, test, and *then* extend.

### ✅ **Week 4: Core Infrastructure & Planning**

**Goals:**
- Set up the FUSE server
- Define client-server protocol
- Set up basic networking

**Tasks:**
- ✅ Finalize command set (e.g., `OPEN`, `READ`, `WRITE`, `LS`, `GETATTR`)
- ✅ Implement initial `networkFS.c` with real files managed by server-side FUSE
- ✅ Make sure server.c runs `fuse_main()` and handles client sockets in parallel (threads or `select`)
- ✅ Create `client.c` that sends commands like `READ file.txt 0 100`
- ✅ Set up socket communication from client to server

**Deliverables:**
- FUSE-based server runs and exposes files
- One client can connect and do basic file operations via socket

---

### ✅ **Week 5: Full File Operations + Concurrency**

**Goals:**
- Implement all required file operations via socket
- Add concurrency support (multiple clients)

**Tasks:**
- ✅ Support `OPEN`, `READ`, `WRITE`, `GETATTR`, `RELEASE` fully over socket
- ✅ Add threading or event loop (`select`, `poll`, or `epoll`) to `server.c`
- ✅ Add proper file locking (if needed)
- ✅ Handle errors (e.g. bad filenames, offset out of range)

**Deliverables:**
- Multiple clients can access files concurrently
- Operations behave correctly under race conditions

---

### ✅ **Week 6: Protocol Refinement & Edge Cases**

**Goals:**
- Improve protocol and robustness

**Tasks:**
- ✅ Switch to a structured protocol (e.g. JSON or fixed headers)
- ✅ Add support for:
  - Binary-safe read/write
  - Proper disconnect handling
  - Timeouts and retries
- ✅ Ensure FUSE operations (like `readdir`, `stat`, etc.) can pull metadata from internal structures

**Deliverables:**
- Clients don't hang or crash on failure
- Protocol is well-defined and extensible

---

### ✅ **Week 7: Security, Deployment & Polish**

**Goals:**
- Final stability, security, and deployment readiness

**Tasks:**
- ✅ Add authentication (token or username/password)
- ✅ Add basic encryption (TLS via `stunnel`, `openssl`, or native)
- ✅ Allow server IP configuration via flags/env
- ✅ Set up deployment scripts (e.g. systemd service for server)

**Deliverables:**
- Secure, authenticated, multi-client shared virtual filesystem
- Deployed on public server, ready for use


# MUST INSTALL ON LINUX:

    - sudo apt install libfuse3-dev fuse3
    - sudo apt install pkgconf
