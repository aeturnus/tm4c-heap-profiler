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

typedef struct _stat
{
    uint32_t t; // total time
    uint32_t n; // count
} stat;

typedef struct _allocator
{
    const char * name;
    const char * desc;
    const heap_ops * ops;
    // stats
    stat malloc;
    stat calloc;
    stat free;
    stat realloc;
} allocator;

// Knuth shims
struct knuth knuth;
void shim_knuth_init(void)
{
    knuth_init(&knuth, heap_mem, MALLOC_SIZE);
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

void allocator_init(allocator * a)
{
    a->malloc.n = 0;
    a->malloc.t = 0;
    a->calloc.n = 0;
    a->calloc.t = 0;
    a->realloc.n = 0;
    a->realloc.t = 0;
    a->free.n = 0;
    a->free.t = 0;
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
    alloc->malloc.t += diff_timer(start, end);
    alloc->malloc.n += 1;
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
    alloc->calloc.t += diff_timer(start, end);
    alloc->calloc.n += 1;
    return ptr;
}

void * realloc(void * p, size_t size)
{
    uint32_t start = 0;
    uint32_t end = 0;
    void * (* f) (void *, size_t) = alloc->ops->realloc;
    start = start_timer();
    void * ptr = f(ptr, size);
    end = stop_timer();
    alloc->realloc.t += diff_timer(start, end);
    alloc->realloc.n += 1;
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
    alloc->free.t += diff_timer(start, end);
    alloc->free.n += 1;
}

void malloc_stats(void)
{
    printf("Allocator: %s\n", alloc->name);
    printf("%d mallocs, %d frees, %d callocs, %d reallocs\n",
           alloc->malloc.n, alloc->free.n, alloc->calloc.n, alloc->realloc.n);
    printf("Average malloc time: %d cycles\n", alloc->malloc.t / alloc->malloc.n);
    printf("Average free time: %d cycles\n", alloc->free.t / alloc->free.n);
    printf("Average calloc time: %d cycles\n", alloc->calloc.t / alloc->calloc.n);
    printf("Average realloc time: %d cycles\n", alloc->realloc.t / alloc->realloc.n);
}

void malloc_reset(void)
{
    malloc_init(curr_impl);
}