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
#include <sys/socket.h>
#include <sys/un.h>

// ref : http://beej.us/guide/bgipc/output/html/multipage/unixsock.html
// stream sockets : TCP
// datagram sockets : UDP

int main(int argc, char *argv[])
{
    
    //int MSG_SIZE = atoi(argv[1]);
    int MSG_SIZE = (4);

    char recv_buf[MSG_SIZE];
    char * send_buf = (char *) calloc(MSG_SIZE+1, sizeof(char));

    int i;
    for(i=0;i<MSG_SIZE;i++)
	send_buf[i] = 'A';
	
    send_buf[MSG_SIZE] = '\0';
    fprintf(stdout, "Msg is: %s\n", send_buf);

    // set up ends..
    
    
    int sv[2]; /* the pair of socket descriptors */

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) 
	{
        perror("socketpair");
        exit(1);
	}

    if (!fork()) 
	{  
	/* child */
        read(sv[1], &recv_buf, MSG_SIZE+1);
        fprintf(stdout, "Child recvs : %s\n", recv_buf);
        
        write(sv[1], recv_buf, MSG_SIZE+1);
        fprintf(stdout, "Child sent : %s\n", recv_buf);
	} 
	
    else 
	{
	/* parent */
        write(sv[0], send_buf, MSG_SIZE+1);
        fprintf(stdout, "Parent sends : %s\n", send_buf);
	
        read(sv[0], &recv_buf, MSG_SIZE+1);
        fprintf(stdout, "Parent recvs : %s\n", send_buf);
	}

    return 0;
}
