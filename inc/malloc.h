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

typedef struct _heap_stat
{
    uint32_t st; // success time
    uint32_t sn; // success count
    uint32_t ft; // fail time
    uint32_t fn; // fail count
} heap_stat;

typedef struct _heap_stats
{
	// stats
    heap_stat malloc;
    heap_stat calloc;
    heap_stat free;
    heap_stat realloc;
} heap_stats;

void malloc_init(heap_impl impl);
void * malloc(size_t size);
void * calloc(size_t nmemb, size_t size);
void * realloc(void * ptr, size_t size);
void free(void * ptr);
void malloc_reset(void);
heap_stats malloc_stats(void);
void malloc_print_stats(void);
void heap_stats_print(const heap_stats * stats);

#endif//__MALLOC_H__
