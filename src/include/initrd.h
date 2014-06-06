#ifndef INITRD_H
#define INITRD_H
/*
 * initrd.h -- Defines the interface for and structures relating to the initial
 * ramdisk.
 *
 * Written for JamesM's kernel development tutorials.
 */
#include <common.h>
#include <vfs.h>

struct initrd_file_header {
	char *filename[101];
	size_t siz;
	char *data;
};

// Initialises the initial ramdisk. It gets passed the address of the multiboot module,
// and returns a completed filesystem node.
struct vfs_node *	init_initrd(void *addr);

#endif /* ndef INITRD_H */
