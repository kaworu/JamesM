/*
 * vfs.c
 *
 * vfs.c -- Defines the interface for and structures relating to the virtual file
 * system. Written for JamesM's kernel development tutorials.
 */
#include <vfs.h>

struct vfs_node *vfs_root = NULL; /* The root of the filesystem. */

uint32_t
vfs_read(struct vfs_node *node, uint32_t offset, uint32_t size, char *buf)
{

	/* Has the node got a read callback? */
	if (node->read != NULL)
		return node->read(node, offset, size, buf);
	else
		return 0;
}

uint32_t
vfs_write(struct vfs_node *node, uint32_t offset, uint32_t size, char *buf)
{

	if (node->write != NULL)
		return node->write(node, offset, size, buf);
	else
		return 0;
}

uint32_t
vfs_open(struct vfs_node *node, int mode)
{

	if (node->open != NULL)
		return node->open(node, mode);
	else
		return 0;
}

void
vfs_close(struct vfs_node *node)
{

	if (node->close != NULL)
		node->close(node);
}

struct dirent *
vfs_readdir(struct vfs_node *node, uint32_t index)
{

	if (VFS_ISDIR(node) && node->readdir != NULL)
		return node->readdir(node, index);
	else
		return NULL;
}

struct vfs_node *
vfs_finddir(struct vfs_node *node, char *name)
{

	if (VFS_ISDIR(node) && node->finddir != NULL)
		return node->finddir(node, name);
	else
		return NULL;
}
