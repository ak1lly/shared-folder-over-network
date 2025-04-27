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
Mount your FUSE filesystem:
```bash
./myfs /mnt/myshare
```
Check:
- `ls /mnt/myshare`
- `cat /mnt/myshare/hello.txt`

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
