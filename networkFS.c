#define FUSE_USE_VERSION 30
#include <fuse3/fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>

static struct options {
	const char *filename;
	const char *contents;
	int show_help;
} options;

#define OPTION(t, p)                           \
    { t, offsetof(struct options, p), 1 }
static const struct fuse_opt option_spec[] = {
	OPTION("--name=%s", filename),
	OPTION("--contents=%s", contents),
	OPTION("-h", show_help),
	OPTION("--help", show_help),
	FUSE_OPT_END
};

static int network_getattr(const char* path, struct stat *stbuf, struct fuse_file_info *fi) {

    (void) fi; //to work around compiler warnings for unused function parameters

    int ret = 0;

    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) { // get info of directory
        stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
    } else if (strcmp(path+1, options.filename) == 0) { // get info of file
        stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(options.contents);
    } else {
        ret = -ENOENT; //returns "No such file or directory" if otherwise
    }

    return ret;
}

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
    filler(buf, options.filename, NULL, 0, 0);

    return 0;
}

static int network_open(const char *path, struct fuse_file_info *fi)
{
    if (strcmp(path+1, options.filename) != 0) // only files can be executed so wont work if not given a file.
		return -ENOENT;

	if ((fi->flags & O_ACCMODE) != O_RDONLY) // if doesn't have execute permissions on file
		return -EACCES;

	return 0;
}

static int network_read(const char *path, char *buf, size_t size, off_t offset,
    struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;

    if(strcmp(path+1, options.filename) != 0)
        return -ENOENT;

    len = strlen(options.contents);
    if (offset >= len)
        return 0;

    if (offset + size > len)
        size = len - offset;

    memcpy(buf, options.contents + offset, size);

    return size;


}

static struct fuse_operations networkfs_oper = {
    //.init    = network_init,    // when mounting
    //.destroy = netwprk_destroy, // when unmounting
    .getattr = network_getattr, // stats a file
    .readdir = network_readdir, // lists a directory
    .open    = network_open,       // opens a file
    .read    = network_read        // reads contents of file
};

int main(int argc, char *argv[])
{
    int ret;
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    options.filename = strdup("hello.txt"); // setting up hardcoded file within file system
    options.contents = strdup("Hello from FUSE!\n"); // setting up hardcoded content to said file.

    if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
        return 1;

    ret = fuse_main(args.argc, args.argv, &networkfs_oper, NULL);
    fuse_opt_free_args(&args);
    return ret;
}
