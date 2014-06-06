/*
 * main.c -- Defines the C-code kernel entry point, calls initialisation routines.
 * Made for JamesM's tutorials.
 */
#include <common.h>
#include <multiboot.h>
#include <descriptor_tables.h>
#include <monitor.h>
#include <paging.h>
#include <timer.h>
#include <vfs.h>
#include <initrd.h>

#include <heap.h>

struct multiboot;

void
vfs_print_content(void)
{
	// list the contents of /
	int i = 0;
	struct dirent *node = 0;
	while (node = vfs_readdir(vfs_root, i)) {
  		printf("Found file %s", node->name);
  		struct vfs_node *fsnode = vfs_finddir(vfs_root, node->name);

  		if (VFS_ISDIR(fsnode))
    			printf("\n\t(directory)\n");
  		else {
    			printf("\n\t contents:\n");
    			char buf[1024 + 1];
    			uint32_t sz = vfs_read(fsnode, 0, 1024, buf);
			buf[sz] = '\0';
      			printf("%s\n", buf);
  		}
  		i++;
	}
}


int
kern_main(struct multiboot *mboot_ptr)
{
	extern uint32_t placement_address;

	mon_clear();
	(void)printf("+ booting.\n");

	(void)printf("+ descriptor tables init...");
	init_descriptor_tables();
	printf("OK\n");

	(void)printf("+ multiboot...");
	KASSERT("multiboot modules", mboot_ptr->mods_count > 0);
	uint32_t initrd_start = *((uint32_t *)mboot_ptr->mods_addr);
	uint32_t initrd_end   = *((uint32_t *)(mboot_ptr->mods_addr + 4));
	placement_address = initrd_end; // XXX: hacky here
	printf("OK\n");

	(void)printf("+ switching to paged mode...");
	init_paging();
	printf("OK\n");

	(void)printf("+ VFS...");
	vfs_root = init_initrd((void *)initrd_start);
	printf("OK\n");

	vfs_print_content();

	PANIC("end of kern_main()");
	/* NOTREACHED */
	return (42);
}
