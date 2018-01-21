#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <command.h>

void benchmark_random(uint32_t seed, uint32_t size_low, uint32_t size_high, uint32_t actions);
void benchmark_vector(uint32_t actions);

#define DEFAULT_SEED 0xDEADBEEF
#define DEFAULT_ACTIONS (1 << 12)

static
int random_sized(int argc, char ** argv)
{
    uint32_t lo, hi;

    if (argv[0][7] == 's') {
        lo = 1; hi = 128;
    } else {
        lo = 256; hi = (1 << 12);
    }

    uint32_t seed = DEFAULT_SEED;
    uint32_t actions = DEFAULT_ACTIONS;
    if (argc >= 2) {
        sscanf(argv[1], "%d", &seed);
        if (argc >= 3) {
            sscanf(argv[2], "%d", &actions);
        }
    }
    printf("Running random allocation benchmark:\n");
    printf("Seed: %d (0x%08X) \n", seed, seed);
    printf("Allocation sizes: %d to %d bytes:\n", lo, hi);
    printf("Number of functions called: %d \n", actions);
    benchmark_random(seed, lo, hi, actions);
    return 0;
}

static
int vector_push(int argc, char ** argv)
{
    uint32_t amount = 4096;
    if (argc >= 2) {
        sscanf(argv[1], "%d", &amount);
    }

    benchmark_vector(amount);
}

#define ARRAY_LEN(x) (sizeof(x)/sizeof(x[0]))

static
const command cmds[] =
{
    {"random-sm", "[seed (dec)] [num actions]", "random small (1B - 128B) allocations", random_sized},
    {"random-lg", "[seed (dec)] [num actions]", "random large (256B - 4KB) allocations", random_sized},
    {"vector", "[num pushes (def 4096)]", "Pushes random ints into libbtn's vector", vector_push},
};


static
void print_help(void)
{
    printf("benchmark <benchmark name>:\n");
    print_commands(cmds, ARRAY_LEN(cmds));
}


int cmd_benchmark(int argc, char ** argv)
{
    if (argc < 2) {
        printf("Please provide benchmark name.\n");
        print_help();
        return 1;
    }

    int ran = 0;
    for (int i = 0; i < ARRAY_LEN(cmds); ++i) {
        if (strcmp(cmds[i].cmd, argv[1]) == 0) {
            ran = 1;
            // start from the passed in benchmark
            malloc_reset();
            cmds[i].func(argc - 1, &argv[1]);
        }
    }

    int ret = 0;
    if (ran) {
        malloc_stats();
    } else {
        ret = 1;
        printf("Unrecognized benchmark: \"%s\"\n", argv[1]);
        print_help();
    }

    return ret;
}