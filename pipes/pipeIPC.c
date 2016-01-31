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

// src: http://beej.us/guide/bgipc/output/html/multipage/pipes.html

int main(int argc, char *argv[])
{

int way1[2];
int way2[2];

int MSG_SIZE = atoi(argv[1]);
//int MSG_SIZE = (4);

char recv_buf[MSG_SIZE];
char * send_msg = (char *) calloc(MSG_SIZE+1, sizeof(char));

int i;
for(i=0;i<MSG_SIZE;i++)
	send_msg[i] = 'A';
	
send_msg[MSG_SIZE] = '\0';

fprintf(stdout, "Msg is: %s\n", send_msg);

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
	fprintf(stdout, "Child reads : %s\n", recv_buf);
	
	write( way2[1], recv_buf, strlen(recv_buf)+1); /* write to parent */ 
	fprintf(stdout, "Child sent : %s\n", recv_buf);
	}
else
	{  		
	close(way1[0]); 
	close(way2[1]); 
	
	write( way1[1], send_msg, strlen(send_msg)+1); /* write to child */ 
	fprintf(stdout, "Parent sends : %s\n", send_msg);
	
	read( way2[0], recv_buf, MSG_SIZE+1); /* read from child */	
	fprintf(stdout, "Parent recvs : %s\n", send_msg);
	}


return 0;
}
