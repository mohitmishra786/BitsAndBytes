#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

void m_init(size_t size);
void *m_malloc(size_t size);
void m_free(void *ptr);
void *is_ptr(void *p);
void gc();
void print_heap();

#endif
