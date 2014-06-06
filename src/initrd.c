/*
 * initrd.c -- Defines the interface for and structures relating to the initial
 * ramdisk.
 *
 * Written for JamesM's kernel development tutorials.
 */
#include <tar.h>
#include <initrd.h>


static struct tar_fileQ	*tar_files;
static struct vfs_node	*initrd_root;  /* Our root directory node. */
static struct vfs_node	*initrd_dev;   /* We also add a directory node for /dev, so we can mount devfs later on. */
static struct vfs_node	*root_nodes;   /* List of file nodes. */
static size_t		 nroot_nodes;  /* Number of file nodes. */
static struct dirent	 dirent;


static uint32_t
initrd_read(struct vfs_node *node, uint32_t offset, uint32_t size, char *buffer)
{

	/* XXX: no check at *all* */
	struct tar_file *tf = TAILQ_FIRST(tar_files);
	do {
		if (strcmp(tf->tf_filename, node->name) == 0)
			break;
	} while (tf = TAILQ_NEXT(tf, entries));
	if (tf == NULL || offset > tf->tf_datasiz)
		return 0;
	if (offset + size > tf->tf_datasiz)
		size = tf->tf_datasiz - offset;
	(void)memcpy(buffer, tf->tf_data + offset, size);
	return size;
}

static struct dirent *
initrd_readdir(struct vfs_node *node, uint32_t index)
{
   if (node == initrd_root && index == 0)
   {
     memcpy(dirent.name, "dev", 4);
     dirent.name[3] = 0; // Make sure the string is NULL-terminated.
     dirent.inode = 0;
     return &dirent;
   }

   if (index-1 >= nroot_nodes)
       return 0;
   printf("found %s for index=%d\n", root_nodes[index-1].name, index);
   memcpy(dirent.name, root_nodes[index-1].name, strlen(root_nodes[index-1].name) + 1);
   dirent.name[strlen(root_nodes[index-1].name)] = 0; // Make sure the string is NULL-terminated.
   dirent.inode = root_nodes[index-1].inode;
   return &dirent;
}

static struct vfs_node *initrd_finddir(struct vfs_node *node, char *name)
{
   if (node == initrd_root &&
       !strcmp(name, "dev") )
       return initrd_dev;

   int i;
   for (i = 0; i < nroot_nodes; i++)
       if (!strcmp(name, root_nodes[i].name))
           return &root_nodes[i];
   return 0;
}

struct vfs_node *
init_initrd(void *addr)
{
	// Initialise the root directory.
	initrd_root = kmalloc(sizeof(struct vfs_node));
	memcpy(initrd_root->name, "initrd", 7);
	initrd_root->mask = initrd_root->uid = initrd_root->gid = initrd_root->inode = initrd_root->length = 0;
	initrd_root->flags = VFS_DIRECTORY;
	initrd_root->read = 0;
	initrd_root->write = 0;
	initrd_root->open = 0;
	initrd_root->close = 0;
	initrd_root->readdir = &initrd_readdir;
	initrd_root->finddir = &initrd_finddir;
	initrd_root->ptr = 0;
	initrd_root->impl = 0;

	// Initialise the /dev directory (required!)
	initrd_dev = kmalloc(sizeof(struct vfs_node));
	memcpy(initrd_dev->name, "dev", 4);
	initrd_dev->mask = initrd_dev->uid = initrd_dev->gid = initrd_dev->inode = initrd_dev->length = 0;
	initrd_dev->flags = VFS_DIRECTORY;
	initrd_dev->read = 0;
	initrd_dev->write = 0;
	initrd_dev->open = 0;
	initrd_dev->close = 0;
	initrd_dev->readdir = &initrd_readdir;
	initrd_dev->finddir = &initrd_finddir;
	initrd_dev->ptr = 0;
	initrd_dev->impl = 0;

	struct tar_file *tf;
	int x = tar_parse_mem(addr, &tar_files);
	if (x == -1)
		PANIC("tar_parse_mem");

	root_nodes = kmalloc(sizeof(struct vfs_node) * x);
	nroot_nodes = x;

	// For every file...
	int i = 0;
	TAILQ_FOREACH(tf, tar_files, entries) {
		// Create a new file node.
		memcpy(root_nodes[i].name, tf->tf_filename, strlen(tf->tf_filename) + 1);
		root_nodes[i].mask = root_nodes[i].uid = root_nodes[i].gid = 0;
		root_nodes[i].length = tf->tf_datasiz;
		root_nodes[i].inode = i;
		root_nodes[i].flags = VFS_FILE;
		root_nodes[i].read = &initrd_read;
		root_nodes[i].write = 0;
		root_nodes[i].readdir = 0;
		root_nodes[i].finddir = 0;
		root_nodes[i].open = 0;
		root_nodes[i].close = 0;
		root_nodes[i].impl = 0;
		i++;
	}
	return initrd_root;
}
