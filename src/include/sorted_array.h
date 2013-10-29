#ifndef SORTED_ARRAY_H
#define SORTED_ARRAY_H
// sorted_array.h -- Interface for creating, inserting and deleting
// from ordered arrays.
// Written for JamesM's kernel development tutorials.

#include "common.h"

/*
 * return an integer greater than, equal to, or less than 0, according as the
 * first argument is greater than, equal to, or less than the second argument.
 */
typedef int (*cmp_func_t)(void *, void *);

struct sorted_array {
	void	**sa_array;
	size_t	sa_size;
	size_t	sa_maxsize;
	cmp_func_t sa_cmp;
};

/*
 * A default cmp function.
*/
int default_cmp_func(void *a, void *b);

/*
 * Create an sorted array.
 */
struct sorted_array	new_sorted_array(size_t maxsize, cmp_func_t cmp);
struct sorted_array	place_sorted_array(void *addr, size_t maxsize, cmp_func_t cmp);

/**
  Destroy an sorted array.
**/
void	delete_sorted_array(struct sorted_array *a);

/**
  Add an item into the array.
**/
void	insert_sorted_array(struct sorted_array *a, void *el);

/**
  Lookup the item at index i.
**/
void	*lookup_sorted_array(struct sorted_array *a, uint32_t i);

/**
  Deletes the item at location i from the array.
**/
void	remove_sorted_array(struct sorted_array *a, uint32_t i);

#endif /* ndef SORTED_ARRAY_H */
