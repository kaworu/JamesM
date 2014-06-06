#ifndef VFS_H
#define VFS_H
/*
 * vfs.h -- VFS stuff.
 *
 * Written for JamesM's kernel development tutorials.
 */
#include <common.h>


#define VFS_FILE        0x01
#define VFS_DIRECTORY   0x02
#define VFS_CHARDEVICE  0x03
#define VFS_BLOCKDEVICE 0x04
#define VFS_PIPE        0x05
#define VFS_SYMLINK     0x06
#define VFS_MOUNTPOINT  0x08 /* Is the file an active mountpoint? */
/* Notice that VFS_MOUNTPOINT is given the value 8, not 7. This is so that it can
   be bitwise-OR'd in with VFS_DIRECTORY */
#define	VFS_ISDIR(node)	(((node)->flags & 0x7) == VFS_DIRECTORY)


struct vfs_node;

typedef uint32_t (*vfs_read_func)(struct vfs_node *node, uint32_t offset, uint32_t size, char *buf);
typedef uint32_t (*vfs_write_func)(struct vfs_node *node, uint32_t offset, uint32_t size, char *buf);
typedef uint32_t (*vfs_open_func)(struct vfs_node *node, int mode);
typedef void (*vfs_close_func)(struct vfs_node *node);
typedef struct dirent * (*vfs_readdir_func)(struct vfs_node *node, uint32_t index);
typedef struct vfs_node * (*vfs_finddir_func)(struct vfs_node *node, char *name);


struct vfs_node {
	char name[256];     /* The filename. */
	uint32_t mask;        /* The permissions mask. */
	uint32_t uid;         /* The owning user. */
	uint32_t gid;         /* The owning group. */
	uint32_t flags;       /* Includes the node type. See #defines above. */
	uint32_t inode;       /* This is device-specific - provides a way for a filesystem to identify files. */
	uint32_t length;      /* Size of the file, in bytes. */
	uint32_t impl;        /* An implementation-defined number. */
	vfs_read_func read;
	vfs_write_func write;
	vfs_open_func open;
	vfs_close_func close;
	vfs_readdir_func readdir;
	vfs_finddir_func finddir;
	struct vfs_node *ptr; /* Used by mountpoints and symlinks. */
};

struct dirent /* One of these is returned by the readdir call, according to POSIX. */
{
	char name[128]; /* Filename. */
	uint32_t inode;     /* Inode number. Required by POSIX. */
};


extern struct vfs_node *vfs_root; /* The root of the filesystem. */


uint32_t vfs_read(struct vfs_node *node, uint32_t offset, uint32_t size, char *buf);
uint32_t vfs_write(struct vfs_node *node, uint32_t offset, uint32_t size, char *buf);
uint32_t vfs_open(struct vfs_node *node, int mode);
void vfs_close(struct vfs_node *node);
struct dirent * vfs_readdir(struct vfs_node *node, uint32_t index);
struct vfs_node * vfs_finddir(struct vfs_node *node, char *name);

#endif /* ndef VFS_H */
