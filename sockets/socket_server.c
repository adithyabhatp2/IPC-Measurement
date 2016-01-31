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


void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}


int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */
    

    if (argc != 3)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port> <size>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg:  local port */
    int MSG_SIZE = atoi(argv[2]); /* Second arg: string to echo */
    int RCVBUFSIZE = 600301;
    
    char recv_buf[RCVBUFSIZE];        /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */
    int totalBytesRcvd;
    
    char * send_msg = (char *) calloc(MSG_SIZE+1, sizeof(char));
    
    int i;
    for(i=0;i<MSG_SIZE;i++)
		send_msg[i] = 'A';
    send_msg[MSG_SIZE] = '\0';
    
    
    
    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
      
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, 2) < 0)
        DieWithError("listen() failed");

        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, 
                               &clntLen)) < 0)
            {
		DieWithError("accept() failed");
	    }

        /* clntSock is connected to a client! */

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

    // start timing..
    
    /* Receive message from client */
    recvMsgSize = recv(clntSock, recv_buf, RCVBUFSIZE, 0);
    totalBytesRcvd = recvMsgSize;
    
    //printf("Received.. %s\n", recv_buf);
    printf("Received Msg Size.. %d\n", recvMsgSize);
    
    while (recvMsgSize > 0)      /* zero indicates end of transmission */
    {
	//send(clntSock, send_msg, recvMsgSize, 0);
         
        /* See if there is more data to receive */
        if ((recvMsgSize = recv(clntSock, recv_buf, RCVBUFSIZE, 0)) < 0)
            DieWithError("recv() failed");
	else
	{
	    //printf("Received.. %s\n", recv_buf);
	    printf("Received Msg Size.. %d\n", recvMsgSize);
	    totalBytesRcvd+=recvMsgSize;
	}
    }
    shutdown(clntSock, SHUT_RD);
    printf("Total Received: %d\n", totalBytesRcvd);
    printf("Going to send\n");
    
    
    int sentCount = send(clntSock, send_msg, MSG_SIZE, 0);
    
    // END TIMING
    
    if (sentCount != MSG_SIZE)
        DieWithError("send() sent a different number of bytes than expected");

    
    close(clntSock);    /* Close client socket */
    shutdown(servSock, 2);
    
    
    
    return 0;
}
