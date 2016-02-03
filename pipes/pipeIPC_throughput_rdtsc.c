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

// ref: http://beej.us/guide/bgipc/output/html/multipage/pipes.html

static __inline__ unsigned long GetCC() {
    unsigned a, d;
    asm volatile("rdtsc" : "=a" (a), "=d" (d));
    return ((unsigned long) a) | (((unsigned long) d) << 32);
}

int main(int argc, char *argv[])
{
    // TIME STUFFF
    unsigned long long start, end, cycles_elapsed;
    double nanoseconds;
    cpu_set_t cpuMask;

    if(argc!=2)
    {
        printf("Usage: %s <MSG_SIZE>\n", argv[0]);
        exit(0);
    }

    // TIME STUFFF
    clockid_t clk_id;
    struct timespec tp_start, tp_end, res;
    int time_elapsed_sec;
    long long time_elapsed_nsec;
    int retVal= 0;
    clk_id = CLOCK_MONOTONIC;
    long BILLION = 1000000000L;


    int MSG_SIZE = atoi(argv[1]);
    //int MSG_SIZE = (4);

    char recv_buf[1];
    char * send_msg = (char *) calloc(MSG_SIZE+1, sizeof(char));

    int i;
    for(i=0;i<MSG_SIZE;i++)
        send_msg[i] = 'A';

    send_msg[MSG_SIZE] = '\0';
    //fprintf(stdout, "Msg is: %s\n", send_msg);

    recv_buf[0] = -60;


    int way1[2];
    int way2[2];

    if(pipe(way1) == -1 || pipe(way2) == -1)
    {
        perror("pipe failed");
        exit(1);
    }

    if ( fork() == 0 )
    { 
        /* child */
        close(way1[1]); 
        close(way2[0]); 

        read( way1[0], send_msg, MSG_SIZE+1); /* read from parent */
        //fprintf(stdout, "Child recvs : %s of size %ld\n", recv_buf, strlen(recv_buf));
        //fprintf(stdout, "Child recvs msg of size %ld\n", strlen(recv_buf));

        write( way2[1], recv_buf, 1); /* write to parent */ 
        //fprintf(stdout, "Child sent : %s\n", recv_buf);
        //fprintf(stdout, "Child sends msg of size %ld\n", strlen(recv_buf));
    }
    else
    {  		
        close(way1[0]); 
        close(way2[1]); 

        //Set to one CPU
        CPU_ZERO(&cpuMask);
        CPU_SET(0, &cpuMask);
        sched_setaffinity(0, sizeof(cpuMask), &cpuMask);

        // START TIME
        start = GetCC();
        write( way1[1], send_msg, strlen(send_msg)+1); /* write to child */ 
        //fprintf(stdout, "Parent sends : %s\n", send_msg);
        //fprintf(stdout, "Parent sends msg of size %ld\n", strlen(send_msg));

        read( way2[0], recv_buf, 1); /* read from child */	
        end = GetCC();
        // END TIME


        cycles_elapsed = end - start;
        nanoseconds = ((double) cycles_elapsed) / 3.392061000;

        //fprintf(stdout, "Time elapsed sec: %d, nsec: %ld\n", time_elapsed_sec, time_elapsed_nsec);
        //fprintf(stdout, "Time elapsed in nanosecs : %lld\n",((BILLION*time_elapsed_sec)+time_elapsed_nsec)/2);

        // This format should make for easy batch running..

        printf("%d\t%lld\n", MSG_SIZE, ((BILLION*time_elapsed_sec)+time_elapsed_nsec));

    }




    return 0;
}
