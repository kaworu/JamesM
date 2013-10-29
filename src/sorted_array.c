// sorted_array.c -- Implementation for creating, inserting and deleting
// from ordered arrays.
// Written for JamesM's kernel development tutorials.

#include <sorted_array.h>


int default_cmp_func(void *a, void *b)
{

	return (a - b);
}


struct sorted_array
new_sorted_array(size_t maxsize, cmp_func_t cmp)
{
	struct sorted_array a;
	a.sa_array = kmalloc0(maxsize * sizeof(void *));
	if (a.sa_array == NULL)
		PANIC("kmalloc");
	a.sa_size = 0;
	a.sa_maxsize = maxsize;
	a.sa_cmp = cmp;

	return (a);
}


struct sorted_array place_sorted_array(void *addr, size_t maxsize, cmp_func_t cmp)
{
	struct sorted_array a;

	a.sa_array = addr;
	bzero(a.sa_array, maxsize * sizeof(void *));
	a.sa_size = 0;
	a.sa_maxsize = maxsize;
	a.sa_cmp = cmp;

	return (a);
}


void
delete_sorted_array(struct sorted_array *a)
{

	kfree(a->sa_array);
}


void
insert_sorted_array(struct sorted_array *a, void *el)
{
	uint32_t iterator = 0;

	KASSERT("has a cmp function", a->sa_cmp != NULL);
	KASSERT("has enough room", a->sa_size < a->sa_maxsize);

	while (iterator < a->sa_size && a->sa_cmp(a->sa_array[iterator], el) < 0)
		iterator++;

	if (iterator == a->sa_size) // just add at the end.
		a->sa_array[a->sa_size++] = el;
	else {
		void *tmp = a->sa_array[iterator];
		a->sa_array[iterator] = el;
		while (iterator < a->sa_size) {
			void *tmp2 = a->sa_array[++iterator];
			a->sa_array[iterator] = tmp;
			tmp = tmp2;
		}
		a->sa_size++;
	}
}


void *
lookup_sorted_array(struct sorted_array *a, uint32_t i)
{

	KASSERT("index in range", i < a->sa_size);
	return a->sa_array[i];
}


void
remove_sorted_array(struct sorted_array *a , uint32_t i)
{

	KASSERT("index in range", i < a->sa_size);

	while (i < a->sa_size) {
		a->sa_array[i] = a->sa_array[i + 1];
		i++;
	}
	a->sa_size--;
}
