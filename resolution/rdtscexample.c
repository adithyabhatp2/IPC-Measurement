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
    asm volatile("rdtsc" : "=a" (a), "=d" (d));
    return ((unsigned long) a) | (((unsigned long) d) << 32);
}

int main(int argc, char *argv[])
{

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

    fprintf(stdout, "Cycles elapsed: %ld\n", cycles_elapsed);
    per_second = ((double) cycles_elapsed) / 2.0;
    fprintf(stdout, "Cycles per second: %f\n", per_second);
    seconds = ((double) cycles_elapsed) / 3392061000.0;
    fprintf(stdout, "Seconds: %f\n", seconds);
    nanoseconds = ((double) cycles_elapsed) / 3.3920610000;
    fprintf(stdout, "NanoSeconds: %f\n", nanoseconds);

    return 0;
}
