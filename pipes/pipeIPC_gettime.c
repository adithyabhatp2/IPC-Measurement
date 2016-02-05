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

// ref: http://beej.us/guide/bgipc/output/html/multipage/pipes.html

int main(int argc, char *argv[])
{

if(argc!=2)
	{
	printf("Usage: %s <MSG_SIZE>\n", argv[0]);
	exit(0);
	}

// GETTIME STUFFF
clockid_t clk_id;
struct timespec tp_start, tp_end;
int time_elapsed_sec;
long long time_elapsed_nsec;

clk_id = CLOCK_MONOTONIC;
long BILLION = 1000000000L;
// END GETTIME STUFF

int MSG_SIZE = atoi(argv[1]);
//int MSG_SIZE = (4);

char recv_buf[MSG_SIZE];
char * send_msg = (char *) calloc(MSG_SIZE+1, sizeof(char));

int i;
for(i=0;i<MSG_SIZE;i++)
	send_msg[i] = 'A';
	
send_msg[MSG_SIZE] = '\0';
//fprintf(stdout, "Msg is: %s\n", send_msg);


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
	
	int i=0;
	for(i=0;i<1000;i++)
		{
		read( way1[0], recv_buf, MSG_SIZE+1); /* read from parent */
		//fprintf(stdout, "Child recvs : %s of size %ld\n", recv_buf, strlen(recv_buf));
		//fprintf(stdout, "Child recvs msg of size %ld\n", strlen(recv_buf));

		}
		write( way2[1], recv_buf, 1); /* write to parent */ 
		//fprintf(stdout, "Child sent : %s\n", recv_buf);
		//fprintf(stdout, "Child sends msg of size %ld\n", strlen(recv_buf));
	}
else
	{  		
	close(way1[0]); 
	close(way2[1]); 
	
	// START TIME
	clock_gettime(clk_id, &tp_start);
	int i=0;
	for(i=0;i<1000;i++)
		{
		
		write( way1[1], send_msg, strlen(send_msg)+1); /* write to child */ 
		//fprintf(stdout, "Parent sends : %s\n", send_msg);
		//fprintf(stdout, "Parent sends msg of size %ld\n", strlen(send_msg));
		
		// END TIME
		
		//fprintf(stdout, "Parent recvs : %s of size %ld\n", send_msg, strlen(send_msg));
		//fprintf(stdout, "Parent recvs msg of size %ld\n", strlen(send_msg));
		
		// This format should make for easy batch running..
		


		//fprintf(stdout, "Time elapsed sec: %d, nsec: %ld\n", time_elapsed_sec, time_elapsed_nsec);
		//fprintf(stdout, "Time elapsed in nanosecs : %lld\n",((BILLION*time_elapsed_sec)+time_elapsed_nsec)/2);
		
		}
		
		read( way2[0], recv_buf, 1); /* read from child */	
		// PRINT GETTIME
		clock_gettime(clk_id, &tp_end);
		time_elapsed_sec = (tp_end.tv_sec - tp_start.tv_sec);
		time_elapsed_nsec = (tp_end.tv_nsec - tp_start.tv_nsec);
		
		printf("%lld\n", ((BILLION*time_elapsed_sec)+time_elapsed_nsec)/1000);
		// END PRINT GETTIME
		
	
	}
	



return 0;
}
