// main.c
// Runs on TM4C1294 (and hopefully TM4C123)

#include <stdint.h>
#include "UART.h"
#include <stdio.h>
#include "SysTick.h"
#include "malloc.h"
#include "heap.h"
#include "Random.h"

void init_systick(void)
{
    SysTick_ChangeReload(0xFFFFFF);
}

// cool trick: https://stackoverflow.com/questions/2410976/how-to-define-a-string-literal-in-gcc-command-line
// wanted to pass CHIP_NAME as a string literal, but Keil didn't
// like the escaped quotes
#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

extern void shell(void);
int main(void)
{
    UART_Init();              // initialize UART
    init_systick();
    malloc_init(IMPL_BRANDON_KNUTH);
    Random_Init(0xDEADBEEF);
    
    printf("\n====================================\n");
    printf("Dynamic memory allocation profiler\n");
    printf("Chip: " STRINGIZE_VALUE_OF(CHIP_NAME) "\n");
    printf("Heap size: %d bytes\n", MALLOC_SIZE);
    printf("====================================\n");
    
    shell();
}