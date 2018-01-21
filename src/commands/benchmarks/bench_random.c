#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <malloc.h>
#include <Random.h>

extern uint32_t M;
static
uint32_t rand(void)
{
    uint32_t val = Random();
    val >>= 8;
    return val;
}

static
uint32_t rand_range(uint32_t lo, uint32_t hi)
{
    uint32_t val = Random();
    val >>= 8;

    // move it to the correct range
    val = val % (hi + 1 - lo) + lo;

    return val;
}

static
void rand_set_state(uint32_t state)
{
    M = state;
}

static
uint32_t rand_get_state(void)
{
    return M;
}

#define PTRS 256

static
uint32_t find_empty_ptr(void * ptrs[PTRS])
{
    for (uint32_t i = 0; i < PTRS; ++i) {
        if (ptrs[i] == NULL)
            return i;
    }
    return -1;
}

// assumption: there is at least one taken pointer
static
uint32_t find_taken_state = 0;

static
void set_find_taken_seed(uint32_t seed)
{
    find_taken_state = seed;
}

static
uint32_t find_taken_ptr(void * ptrs[PTRS])
{
    uint32_t save_state = rand_get_state();
    rand_set_state(find_taken_state);
    void * ptr = NULL;
    uint32_t idx = 0;
    while (ptr == NULL) {
        idx = rand() % PTRS;
        ptr = ptrs[idx];
    }
    find_taken_state = rand_get_state();
    rand_set_state(save_state);
    return idx;
}

void benchmark_random(uint32_t seed, uint32_t size_low, uint32_t size_high, uint32_t actions)
{
    rand_set_state(seed);
    set_find_taken_seed(~seed);
    
    static void * ptrs[PTRS];
    for (int i = 0; i < PTRS; ++i) {
        ptrs[i] = 0x0;
    }
        
    // allocate pseudo-random number of bytes
    uint32_t mallocs = 0;
    uint32_t frees = 0;
    for (int i=0; i < actions; ++i) {
        uint32_t r = rand() % 2;
        
        if ((r == 0 || mallocs == frees) && mallocs - frees < PTRS) {
            // malloc if 0, or no allocations, or too many allocations
            uint32_t ptr_idx = find_empty_ptr(ptrs);
            uint32_t size = rand_range(size_low, size_high);
            size = (size > 0) ? size : 1;
            void * ptr = malloc(size);
            
            if (ptr != NULL) {
                ptrs[ptr_idx] = ptr;
                ++mallocs;
            }
        } else {
            uint32_t ptr_idx = find_taken_ptr(ptrs);
            void * ptr = ptrs[ptr_idx];
            free(ptr);
            ptrs[ptr_idx] = NULL;
            ++frees;
        }
    }
}