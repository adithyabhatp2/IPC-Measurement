#include<stdio.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#define _GNU_SOURCE
#include <sched.h>
#include <time.h>

#define BILLION 1000000000L

int x = 0;

static __inline__ unsigned long GetCC() {
    unsigned a, d;
    asm volatile("rdtsc" : "=a" (a), "=d" (d));
    return ((unsigned long) a) | (((unsigned long) d) << 32);
}

int main() {
    unsigned long start, end, cycles_elapsed;
    cpu_set_t cpuMask;
    struct timespec start_clock, end_clock;
    int time_elapsed_sec;
    long time_elapsed_nsec;
    double nanoseconds;

    //This is the CPU frequency on my CSL desktop. We should decide what machine we are testing on.
    double cpuFreq = 3.392061000;

    //Set to one CPU
    CPU_ZERO(&cpuMask);
    CPU_SET(0, &cpuMask);
    sched_setaffinity(0, sizeof(cpuMask), &cpuMask);

    clock_gettime(CLOCK_MONOTONIC, &start_clock);
    start = GetCC();
    for(x = 0; x < 100000000; x++)
    end = GetCC();
    clock_gettime(CLOCK_MONOTONIC, &end_clock);

    time_elapsed_sec = (end_clock.tv_sec - start_clock.tv_sec);
    time_elapsed_nsec = (end_clock.tv_nsec - start_clock.tv_nsec);
    cycles_elapsed = end - start;

    fprintf(stdout, "%ld Clock\n",(BILLION*time_elapsed_sec)+time_elapsed_nsec);
    nanoseconds = ((double) cycles_elapsed) / cpuFreq;
    fprintf(stdout, "%.0f RDTSC\n", nanoseconds);

    //printf("%lu - %lu = %lu\n", end, start, (end - start));
    //printf("%ld - %ld = %ld\n", end_clock.tv_nsec, start_clock.tv_nsec, (end_clock.tv_nsec - start_clock.tv_nsec));
    //printf("%ld - %ld = %ld\n", end_clock.tv_sec, start_clock.tv_sec, (end_clock.tv_sec - start_clock.tv_sec));
    //printf("%f\n", ((double) (end - start)) / ((double) (end_clock.tv_sec - start_clock.tv_sec)));

    return 0;
}
