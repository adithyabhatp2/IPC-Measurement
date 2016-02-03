# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
# include <errno.h>
# include <limits.h>

#include <time.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#define _GNU_SOURCE
#include <sched.h>


#define BILLION 1000000000L

static __inline__ unsigned long GetCC() {
    unsigned a, d;
    asm("cpuid"); // disables out of order exec
    asm volatile("rdtsc" : "=a" (a), "=d" (d));
    return ((unsigned long) a) | (((unsigned long) d) << 32);
}

int main(int argc, char *argv[])
{
    
    float cpuFreq=3192517000;
    
    printf("Hello");

    unsigned long start, end, cycles_elapsed;
    cpu_set_t cpuMask;
    double per_second, seconds, nanoseconds;

    //Set to one CPU
    CPU_ZERO(&cpuMask);
    CPU_SET(0, &cpuMask);
    sched_setaffinity(0, sizeof(cpuMask), &cpuMask);

    //retVal = clock_getres(clk_id, &res);
    //fprintf(stdout, "MONOTONIC resolution is sec: %ld, nsec: %ld\n", res.tv_sec, res.tv_nsec);

    start = GetCC();
    sleep(2);
    end = GetCC();

    cycles_elapsed = (end - start);
    per_second = ((double) cycles_elapsed) / 2.0;
    seconds = ((double) cycles_elapsed) / cpuFreq;
    nanoseconds = ((double) cycles_elapsed) / (cpuFreq/1000000000);
    
    fprintf(stdout, "Cycles elapsed: %ld\n", cycles_elapsed);
    fprintf(stdout, "Cycles per second: %f\n", per_second);
    fprintf(stdout, "Seconds: %f\n", seconds);
    printf("Nano : %f\n", nanoseconds);


    return 0;
}
