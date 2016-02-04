//#include <time.h>
#include <stdio.h>
#include <stdint.h>

#ifndef __USE_GNU
#define __USE_GNU
#endif
#define _GNU_SOURCE
#include <sched.h>

/* Compiled & executed with:

    gcc rdtsc_overhead.c -O0 -lrt -o rdtsc_overhead
    ./rdtsc_overhead 100000
*/

static __inline__ unsigned long GetCC() {
    unsigned a, d;
    asm volatile("rdtsc" : "=a" (a), "=d" (d));
    return ((unsigned long) a) | (((unsigned long) d) << 32);
}

int main(int argc, char **args) {
    unsigned long long start, end, dummy;
    cpu_set_t cpuMask;
    double cpuFreq, nanoseconds;
    int n, N;

    //TODO: set cpuFreq to the appropriate value. 
    //This one is completely fictitious
    cpuFreq = 0.331060000000;

    N = atoi(args[1]);

    //Set to one CPU
    CPU_ZERO(&cpuMask);
    CPU_SET(0, &cpuMask);
    sched_setaffinity(0, sizeof(cpuMask), &cpuMask);

    start = GetCC();
    for (n = 0; n < N; ++n) {
        dummy = GetCC();
        dummy = GetCC();
        dummy = GetCC();
        dummy = GetCC();
        dummy = GetCC();
        dummy = GetCC();
        dummy = GetCC();
        dummy = GetCC();
        dummy = GetCC();
        dummy = GetCC();
    }
    end = GetCC();

    printf("Estimated overhead: %.0lf ns\n",
            (end - start) / cpuFreq / (N+1) / 10);
    return 0;
}
