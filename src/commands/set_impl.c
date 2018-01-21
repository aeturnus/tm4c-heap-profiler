#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>

static
void print_help(void)
{
    printf("set-impl <implementation>:\n");
    printf("Implementations:\n");
    printf("    valvano  : Valvanoware's Knuth allocator\n");
    printf("    btn-knuth: Brandon's Knuth + free list allocator\n");
}

int cmd_set_impl(int argc, char ** argv)
{
    if (argc < 2) {
        printf("Please provide an implementation.\n");
        print_help();
        return 1;
    }

    int ret = 0;
    const char * str = argv[1];
    if (strcmp("valvano", str) == 0) {
        malloc_init(IMPL_VALVANO);
        puts("Implementation set to Valvano's");
    } else if (strcmp("btn-knuth", str) == 0) {
        malloc_init(IMPL_BRANDON_KNUTH);
        puts("Implementation set to Brandon's Knuth");
    } else if (strcmp("--help", str) == 0 ||
               strcmp("-h", str) == 0 ||
               strcmp("help", str) == 0) {
        print_help();
    } else {
        printf("Unrecognized implementation: \"%s\"\n", str);
        print_help();
        ret = 2;
    }
    return ret;
}