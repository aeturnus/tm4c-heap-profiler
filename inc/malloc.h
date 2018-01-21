#ifndef __MALLOC_H__
#define __MALLOC_H__
#include <stddef.h>
#include <stdint.h>

#define MALLOC_SIZE 0x10000
extern uint8_t heap_mem[MALLOC_SIZE];


typedef enum _heap_impl
{
    IMPL_VALVANO,
    IMPL_BRANDON_KNUTH
} heap_impl;


void malloc_init(heap_impl impl);
void * malloc(size_t size);
void * calloc(size_t nmemb, size_t size);
void * realloc(void * ptr, size_t size);
void free(void * ptr);

#endif//__MALLOC_H__
