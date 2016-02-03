#include <time.h>
#include <stdio.h>
#include <stdint.h>

/* Compiled & executed with:

    gcc clock_gettime_overhead.c -O0 -lrt -o clock_gettime_overhead
    ./clock_gettime_overhead 100000
*/

int main(int argc, char **args) {
    struct timespec tstart, tend, dummy;
    int n, N;
    N = atoi(args[1]);
    clock_gettime(CLOCK_MONOTONIC_RAW, &tstart);
    for (n = 0; n < N; ++n) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &dummy);
        clock_gettime(CLOCK_MONOTONIC_RAW, &dummy);
        clock_gettime(CLOCK_MONOTONIC_RAW, &dummy);
        clock_gettime(CLOCK_MONOTONIC_RAW, &dummy);
        clock_gettime(CLOCK_MONOTONIC_RAW, &dummy);
        clock_gettime(CLOCK_MONOTONIC_RAW, &dummy);
        clock_gettime(CLOCK_MONOTONIC_RAW, &dummy);
        clock_gettime(CLOCK_MONOTONIC_RAW, &dummy);
        clock_gettime(CLOCK_MONOTONIC_RAW, &dummy);
        clock_gettime(CLOCK_MONOTONIC_RAW, &dummy);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &tend);
    printf("Estimated overhead: %lld ns\n",
            ((int64_t) tend.tv_sec * 1000000000 + (int64_t) tend.tv_nsec
                    - ((int64_t) tstart.tv_sec * 1000000000
                            + (int64_t) tstart.tv_nsec)) / (N+1) / 10);
    return 0;
}
