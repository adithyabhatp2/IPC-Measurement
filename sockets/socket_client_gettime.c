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
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */

// ref : http://cs.baylor.edu/~donahoo/practical/CSockets/code/TCPEchoClient.c

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}


int main(int argc, char *argv[])
{
    
    int sock;                        /* Socket descriptor */
    struct sockaddr_in socketServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() 
                                        and total bytes read */

    if (argc != 4)    /* Test for correct number of arguments */
    {
       fprintf(stderr, "Usage: %s <Server IP> <port> <size> \n",
               argv[0]);
       exit(1);
    }
    
    // GETTIME STUFFF
    clockid_t clk_id;
    struct timespec tp_start, tp_end;
    int time_elapsed_sec;
    long long time_elapsed_nsec;
    long long throughput_nsec;
    clk_id = CLOCK_MONOTONIC;
    long BILLION = 1000000000L;
    // END GETTIME STUFF
    

    servIP = argv[1];             /* First arg: server IP address (dotted quad) */
    echoServPort = atoi(argv[2]); /* Use given port, if any */
    int MSG_SIZE = atoi(argv[3]); /* Second arg: string to echo */
    
    //printf("Port: %d, Size: %d\n", echoServPort, MSG_SIZE);
    
    int RCVBUFSIZE = 600301;

    char recv_buf[RCVBUFSIZE];     /* Buffer for echo string */
    char * send_msg = (char *) calloc(MSG_SIZE+1, sizeof(char));
    
    int i;
    for(i=0;i<MSG_SIZE;i++)
	send_msg[i] = 'A';
    send_msg[MSG_SIZE] = '\0';
    
    
    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&socketServAddr, 0, sizeof(socketServAddr));     /* Zero out structure */
    socketServAddr.sin_family      = AF_INET;             /* Internet address family */
    socketServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    socketServAddr.sin_port        = htons(echoServPort); /* Server port */

    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &socketServAddr, sizeof(socketServAddr)) < 0)
        DieWithError("connect() failed");
    
    
    // START TIMING
    clock_gettime(clk_id, &tp_start);
    
    int sentCount = send(sock, send_msg, MSG_SIZE, 0);
    if (sentCount != MSG_SIZE)
        DieWithError("send() sent a different number of bytes than expected");
    
    // printf("Sent msg.. size:%d\n", sentCount);
    shutdown(sock, SHUT_WR);

    /* Receive the same string back from the server */
    totalBytesRcvd = 0;
    // printf("Received: ");                /* Setup to print the echoed string */
    
    while (totalBytesRcvd < MSG_SIZE)
    {
        /* Receive up to the buffer size (minus 1 to leave space for
           a null terminator) bytes from the sender */
        bytesRcvd = recv(sock, recv_buf, RCVBUFSIZE - 1, 0);
        
		//if (bytesRcvd <= 0)
          //  DieWithError("recv() failed or connection closed prematurely");
        
        totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
        recv_buf[bytesRcvd] = '\0';  /* Terminate the string! */
	
    //printf("%s", recv_buf);      /* Print the echo buffer */
	//printf("Received Msg Size.. %d\n", bytesRcvd);
    }
    clock_gettime(clk_id, &tp_end);
    // END TIMING

    //printf("Total Received: %d\n", totalBytesRcvd);
    
    
    // PRINT GETTIME
    time_elapsed_sec = (tp_end.tv_sec - tp_start.tv_sec);
    time_elapsed_nsec = (tp_end.tv_nsec - tp_start.tv_nsec);
    //printf("%d\t%lld\n", MSG_SIZE, ((BILLION*time_elapsed_sec)+time_elapsed_nsec)/2);
    printf("%lld\n", ((BILLION*time_elapsed_sec)+time_elapsed_nsec)/2);
    // END PRINT GETTIME
    
    // This format should make for easy batch running..
    // printf("%d\t%ld", MSG_SIZE, total_time);
    

    close(sock);
    exit(0);
}

