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
	
	read( way1[0], recv_buf, MSG_SIZE+1); /* read from parent */
	//fprintf(stdout, "Child recvs : %s of size %ld\n", recv_buf, strlen(recv_buf));
	//fprintf(stdout, "Child recvs msg of size %ld\n", strlen(recv_buf));
	
	write( way2[1], recv_buf, strlen(recv_buf)+1); /* write to parent */ 
	//fprintf(stdout, "Child sent : %s\n", recv_buf);
	//fprintf(stdout, "Child sends msg of size %ld\n", strlen(recv_buf));
	}
else
	{  		
	close(way1[0]); 
	close(way2[1]); 
	
	// START TIME
	
	write( way1[1], send_msg, strlen(send_msg)+1); /* write to child */ 
	//fprintf(stdout, "Parent sends : %s\n", send_msg);
	//fprintf(stdout, "Parent sends msg of size %ld\n", strlen(send_msg));
	
	read( way2[0], recv_buf, MSG_SIZE+1); /* read from child */	
	
	// END TIME
	
	//fprintf(stdout, "Parent recvs : %s of size %ld\n", send_msg, strlen(send_msg));
	//fprintf(stdout, "Parent recvs msg of size %ld\n", strlen(send_msg));
	}
	
	// This format should make for easy batch running..
	// printf("%d\t%ld", MSG_SIZE, total_time);


return 0;
}
