#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>

static
void print_help(void)
{
    printf("benchmark <benchmark name>:\n");
    printf("benchmarks:\n");
    printf("    random-small: Valvanoware's Knuth allocator\n");
}

int cmd_benchmark(int argc, char ** argv)
{
    if (argc < 2) {
        printf("Please provide benchmark name.\n");
        print_help();
        return 1;
    }

    int ret = 0;
    return ret;
}