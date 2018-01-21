#ifndef BENCHMARKS_H
#define BENCHMARKS_H

void benchmark_random(uint32_t seed, uint32_t size_low, uint32_t size_high, uint32_t actions);
void benchmark_vector(uint32_t actions);
void benchmark_fixed(uint32_t size, uint32_t actions);

#endif