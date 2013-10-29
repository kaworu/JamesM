#ifndef KMALLOC_H
#define KMALLOC_H
/*
 * kmalloc.h -- Heap memory allocation routines
 * From JamesM's kernel development tutorials.
 */
#include <types.h>

/* all kmalloc0* routines will ensure that the memory requested is filled with
 * 0x0 */
void	*kmalloc(size_t len); /* classic malloc */
void	*kmalloc0(size_t len);
void	*kmalloc_a(size_t len); /* page aligned malloc */
void	*kmalloc0_a(size_t len);
void	*kmalloc_p(size_t len, uint32_t *phys); /* physical address */
void	*kmalloc0_p(size_t len, uint32_t *phys);
void	*kmalloc_ap(size_t len, uint32_t *phys); /* page aligned and physical address */
void	*kmalloc0_ap(size_t len, uint32_t *phys);

void	kfree(void *ptr);

#endif /* ndef KMALLOC_H */
