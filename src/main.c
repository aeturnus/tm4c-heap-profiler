// main.c
// Runs on TM4C1294
// Test the Random function to verify that it is random and
// fair.  Print any interesting statistics to the UART.
// Daniel Valvano
// April 23, 2014

/* This example accompanies the book
   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
   Program 2.8, Section 2.8

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include <stdint.h>
#include "UART.h"
#include <stdio.h>
#include "SysTick.h"
#include "malloc.h"
#include "heap.h"

void init_systick(void)
{
    SysTick_ChangeReload(0xFFFFFF);
}

int main(void)
{
    UART_Init();              // initialize UART
    init_systick();
    malloc_init(IMPL_BRANDON_KNUTH);
    
    printf("\n====================================\n");
    printf("Dynamic memory allocation profiler\n");
    printf("Chip: TM4C1294\n");
    printf("Heap size: %d bytes\n", MALLOC_SIZE);
    printf("====================================\n");
}
