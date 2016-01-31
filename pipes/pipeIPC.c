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



int main(int argc, char *argv[])
{

int pdes[2];

int MSG_SIZE = 6;

char recv_buf[MSG_SIZE];
char send_msg[] = "Hello\0";

if(pipe(pdes) == -1)
	{
		perror("pipe failed");
		exit(1);
	}
	
if ( fork() == 0 )
	{ 
	/* child */
	close(pdes[1]); 
	read( pdes[0], recv_buf, 6); /* read from parent */
	fprintf(stdout, "Child reads : %s\n", recv_buf);
	}
else
	{  		
	close(pdes[0]); 
	write( pdes[1], send_msg, strlen(send_msg)); /* write to child */ 
	fprintf(stdout, "Parent sends : %s\n", send_msg);
	}


return 0;
}
