/*
 * tar.c
 *
 * A (very) basic and naive "tape archive" parser implementation.
 */
#include <tar.h>


/*
 * see
 * http://en.wikipedia.org/wiki/Tar_(computing)#File_header,
 * http://wiki.osdev.org/Tar
 */
struct tar_header {
	char filename[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char chksum[8];
	char typeflag[1];
};


static unsigned int
getsize(const char *in)
{
    unsigned int size = 0;
    unsigned int j;
    unsigned int count = 1;
 
    for (j = 11; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);
 
    return size;
 
}

int
tar_parse_mem(void *addr, struct tar_fileQ **q_p)
{
	char *p = addr;
	struct tar_fileQ *q;
	int i;

	q = kmalloc0(sizeof(struct tar_fileQ));
	if (q == NULL)
		return -1;
	TAILQ_INIT(q);
	for (i = 0; ; i++) {
		struct tar_header *header = (void *)p;
		size_t siz;
		struct tar_file *tf = NULL;

		if (header->filename[0] == '\0') {
			/* we hit the end of the tarball */
			break;
		}

		siz = getsize(header->size);
		tf = kmalloc0(sizeof(struct tar_file));
		if (tf == NULL)
			return -1;
		(void)memcpy(tf->tf_filename, header->filename, 100);
		tf->tf_datasiz = siz;
		tf->tf_data = ((char *)header) + 512;
		TAILQ_INSERT_TAIL(q, tf, entries);

		p += ((siz / 512) + 1) * 512;
		if (siz % 512)
			p += 512;
	}
	*q_p = q;
	return i;
}
