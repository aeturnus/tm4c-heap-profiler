#include <stdint.h>
#include <stddef.h>
#include "SysTick.h"
#include "knuth.h"
#include "heap.h"
#include "malloc.h"

uint8_t heap_mem[MALLOC_SIZE];

typedef struct _heap_ops
{
    void (* init) (void);
    void * (* malloc) (size_t size);
    void * (* calloc) (size_t nmemb, size_t size);
    void * (* realloc) (void * ptr, size_t size);
    void (* free) (void * ptr);
} heap_ops;



typedef struct _allocator
{
    const char * name;
    const char * desc;
    const heap_ops * ops;
    heap_stats stats;
} allocator;

// Knuth shims
struct knuth knuth;
void shim_knuth_init(void)
{
    knuth_init(&knuth, heap_mem, MALLOC_SIZE, 2);
}

void * shim_knuth_malloc(size_t size)
{
    return knuth_malloc(&knuth, size);
}

void * shim_knuth_calloc(size_t nmemb, size_t size)
{
    return knuth_calloc(&knuth, nmemb, size);
}

void * shim_knuth_realloc(void * ptr, size_t size)
{
    return knuth_realloc(&knuth, ptr, size);
}

void shim_knuth_free(void * ptr)
{
    knuth_free(&knuth, ptr);
}

const heap_ops knuth_ops =
{
    .init = shim_knuth_init,
    .malloc = shim_knuth_malloc,
    .realloc = shim_knuth_realloc,
    .calloc = shim_knuth_calloc,
    .free = shim_knuth_free
};
allocator knuth_allocator =
{
    .name = "Brandon's Knuth",
    .desc = "Knuth heap with embedded free list",
    .ops = &knuth_ops
};
//

// valvano
void shim_val_init(void)
{
    Heap_Init();
}

void shim_val_free(void * ptr)
{
    Heap_Free(ptr);
}

const heap_ops val_ops =
{
    .init = shim_val_init,
    .malloc = Heap_Malloc,
    .realloc = Heap_Realloc,
    .calloc = Heap_Calloc,
    .free = shim_val_free
};

allocator val_allocator =
{
    .name = "Valvano",
    .desc = "Valvanoware heap.c, Knuth heap",
    .ops = &val_ops
};
//

void stat_init(heap_stat * stat)
{
    stat->sn = 0;
    stat->fn = 0;
    stat->st = 0;
    stat->ft = 0;
}

void allocator_init(allocator * a)
{
    stat_init(&a->stats.malloc);
    stat_init(&a->stats.realloc);
    stat_init(&a->stats.calloc);
    stat_init(&a->stats.free);
}

static allocator * alloc = NULL;
static heap_impl curr_impl;
void malloc_init(heap_impl impl)
{
    switch(impl)
    {
    case IMPL_VALVANO:
        alloc = &val_allocator;
        break;
    case IMPL_BRANDON_KNUTH:
        alloc = &knuth_allocator;
        break;
    }
    allocator_init (alloc);
    alloc->ops->init();
    curr_impl = impl;
}

static inline
uint32_t start_timer(void)
{
    SysTick_Clear();
    SysTick_Enable();
    return SysTick_Current;
}

static inline
uint32_t stop_timer(void)
{
    SysTick_Disable();
    return SysTick_Current;
}

static inline
uint32_t diff_timer(uint32_t start, uint32_t end)
{
    return start - end;
}

void * malloc(size_t size)
{
    uint32_t start = 0;
    uint32_t end = 0;
    void * (* f) (size_t) = alloc->ops->malloc;
    start = start_timer();
    void * ptr = f(size);
    end = stop_timer();
    
    if (ptr != NULL) {
        alloc->stats.malloc.st += diff_timer(start, end);
        alloc->stats.malloc.sn += 1;
    } else {
        alloc->stats.malloc.ft += diff_timer(start, end);
        alloc->stats.malloc.fn += 1;
    }

    return ptr;
}

void * calloc(size_t nmemb, size_t size)
{
    uint32_t start = 0;
    uint32_t end = 0;
    void * (* f) (size_t, size_t) = alloc->ops->calloc;
    start = start_timer();
    void * ptr = f(nmemb, size);
    end = stop_timer();
    
    if (ptr != NULL) {
        alloc->stats.calloc.st += diff_timer(start, end);
        alloc->stats.calloc.sn += 1;
    } else {
        alloc->stats.calloc.ft += diff_timer(start, end);
        alloc->stats.calloc.fn += 1;
    }
    return ptr;
}

void * realloc(void * ptr, size_t size)
{
    uint32_t start = 0;
    uint32_t end = 0;
    void * (* f) (void *, size_t) = alloc->ops->realloc;
    start = start_timer();
    ptr = f(ptr, size);
    end = stop_timer();
    
    if (ptr != NULL) {
        alloc->stats.realloc.st += diff_timer(start, end);
        alloc->stats.realloc.sn += 1;
    } else {
        alloc->stats.realloc.ft += diff_timer(start, end);
        alloc->stats.realloc.fn += 1;
    }
    return ptr;
}

void free(void * ptr)
{
    uint32_t start = 0;
    uint32_t end = 0;
    void (* f) (void *) = alloc->ops->free;
    start = start_timer();
    f(ptr);
    end = stop_timer();
    
    alloc->stats.free.st += diff_timer(start, end);
    alloc->stats.free.sn += 1;
}

heap_stats malloc_stats(void)
{
    return alloc->stats;
}

void malloc_print_stats(void)
{
    printf("Allocator: %s\n", alloc->name);
    heap_stats_print(&alloc->stats);
}

void heap_stats_print(const heap_stats * stats)
{
    printf("Successful %d mallocs, %d frees, %d callocs, %d reallocs\n",
           stats->malloc.sn, stats->free.sn, stats->calloc.sn, stats->realloc.sn);
    printf("Failed %d mallocs, %d frees, %d callocs, %d reallocs\n",
           stats->malloc.fn, stats->free.fn, stats->calloc.fn, stats->realloc.fn);
    puts("");
    printf("Avg. successful malloc time: %d cycles\n", stats->malloc.st / stats->malloc.sn);
    printf("Avg. successful free time: %d cycles\n", stats->free.st / stats->free.sn);
    printf("Avg. successful calloc time: %d cycles\n", stats->calloc.st / stats->calloc.sn);
    printf("Avg. successful realloc time: %d cycles\n", stats->realloc.st / stats->realloc.sn);
    puts("");
    printf("Avg. failed malloc time: %d cycles\n", stats->malloc.ft / stats->malloc.fn);
    printf("Avg. failed free time: %d cycles\n", stats->free.ft / stats->free.fn);
    printf("Avg. failed calloc time: %d cycles\n", stats->calloc.ft / stats->calloc.fn);
    printf("Avg. failed realloc time: %d cycles\n", stats->realloc.ft / stats->realloc.fn);
}

void malloc_reset(void)
{
    malloc_init(curr_impl);
}