#ifndef TAR_H
#define TAR_H
/*
 * tar.h
 *
 * A (very) basic and naive "tape archive" parser implementation.
 */
#include <common.h>


struct tar_file {
	char	 tf_filename[101];
	size_t	 tf_datasiz;
	char	*tf_data;
	TAILQ_ENTRY(tar_file)	entries;
};
TAILQ_HEAD(tar_fileQ, tar_file);


int	tar_parse_mem(void *addr, struct tar_fileQ **q_p);

#endif /* ndef TAR_H */
