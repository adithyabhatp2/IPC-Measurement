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


#define BILLION 1000000000L

int main(int argc, char *argv[])
{


clockid_t clk_id;
struct timespec tp_start, tp_end, res;
int time_elapsed_sec;
long time_elapsed_nsec;
int retVal= 0;

clk_id = CLOCK_MONOTONIC;

retVal = clock_getres(clk_id, &res);
//fprintf(stdout, "MONOTONIC resolution is sec: %ld, nsec: %ld\n", res.tv_sec, res.tv_nsec);


/*
int i;
for(i=0;i<100;i++)
{
    clock_gettime(clk_id, &tp_start);
    i++;
    i--;
    clock_gettime(clk_id, &tp_end);
    time_elapsed_sec = (tp_end.tv_sec - tp_start.tv_sec);
    time_elapsed_nsec = (tp_end.tv_nsec - tp_start.tv_nsec);
    printf("%ld\n", ((BILLION*time_elapsed_sec)+time_elapsed_nsec));
}
*/

    int i=0;
    
    res.tv_sec = 0;
    for(i=0;i<100000;i++){i++;};
    res.tv_nsec = 1;

    clock_gettime(clk_id, &tp_start);
    clock_gettime(clk_id, &tp_end);
    time_elapsed_sec = (tp_end.tv_sec - tp_start.tv_sec);
    time_elapsed_nsec = (tp_end.tv_nsec - tp_start.tv_nsec);
    printf("%ld\n", ((BILLION*time_elapsed_sec)+time_elapsed_nsec));


/*
fprintf(stdout, "Time elapsed sec: %d, nsec: %ld\n", time_elapsed_sec, time_elapsed_nsec);
fprintf(stdout, "Time elapsed in nanosecs : %ld\n",(BILLION*time_elapsed_sec)+time_elapsed_nsec);
*/



return 0;
}
