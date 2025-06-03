#define FUSE_USE_VERSION 30
#include <fuse3/fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stddef.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 4096

int sock;

// static int network_getattr(const char* path, struct stat *stbuf, struct fuse_file_info *fi) {

//     (void) fi; //to work around compiler warnings for unused function parameters

//     int ret = 0;

//     memset(stbuf, 0, sizeof(struct stat));
//     if (strcmp(path, "/") == 0) { // get info of directory
//         stbuf->st_mode = S_IFDIR | 0755;
// 		stbuf->st_nlink = 2;
//     } else if (strcmp(path+1, options.filename) == 0) { // get info of file
//         stbuf->st_mode = S_IFREG | 0444;
// 		stbuf->st_nlink = 1;
// 		stbuf->st_size = strlen(options.contents);
//     } else {
//         ret = -ENOENT; //returns "No such file or directory" if otherwise
//     }

//     return ret;
// }

static int network_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi,
			 enum fuse_readdir_flags flags)
{
    (void) offset;
	(void) fi;
	(void) flags;

    if (strcmp(path, "/") != 0) //if not a directory
        return -ENOENT;

    // fills directory entries into directory structure of a given directiry path
    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    
    char cmd[BUFFER_SIZE];
    sprintf(cmd, "ls %s", path[1] ? path+1 : ".");
    send(sock, cmd, strlen(cmd), 0);
    char out[BUFFER_SIZE];
    int n = recv(sock, out, BUFFER_SIZE-1, 0);
    if (n <= 0) 
        return 0;
    char* token = strtok(out, "\n");
    while(token) {
        filler(buf, token, NULL, 0, 0);
        token = strtok(NULL, "\n");
    }

    return 0;
}

// static int network_open(const char *path, struct fuse_file_info *fi)
// {
//     if (strcmp(path+1, options.filename) != 0) // only files can be executed so wont work if not given a file.
// 		return -ENOENT;

// 	if ((fi->flags & O_ACCMODE) != O_RDONLY) // if doesn't have execute permissions on file
// 		return -EACCES;

// 	return 0;
// }

// static int network_read(const char *path, char *buf, size_t size, off_t offset,
//     struct fuse_file_info *fi)
// {
//     size_t len;
//     (void) fi;

//     if(strcmp(path+1, options.filename) != 0)
//         return -ENOENT;

//     len = strlen(options.contents);
//     if (offset >= len)
//         return 0;

//     if (offset + size > len)
//         size = len - offset;

//     memcpy(buf, options.contents + offset, size);

//     return size;


// }

static struct fuse_operations networkfs_oper = {
    // .getattr = network_getattr, // stats a file
    .readdir = network_readdir, // lists a directory
    // .open    = network_open,       // opens a file
    // .read    = network_read        // reads contents of file
};

int main(int argc, char *argv[]) {
    // Connecting to server...
    struct sockaddr_in serv_addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));


    // Intiating Filesystem in USErspace 

    int ret = fuse_main(argc - 1, &argv[1], &networkfs_oper, NULL);
    return ret;
}
