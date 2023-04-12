#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "banking.h"

#define NUM_ACCTS   100

typedef struct
{
    unsigned int 	balance;
    pthread_mutex_t mutex;
} sBANK_ACCT_DATA;

sBANK_ACCT_DATA acctData[NUM_ACCTS];

int setupTCPServer(unsigned int portNum, int maxConn)
{
    int servSocket;
    struct sockaddr_in servAddr;

    /* Setup address */
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(0);    /* Accept connections on an IP address */
    servAddr.sin_port = htons(portNum);

    /* Create socket */
    if((servSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("Failed to create socket; Error %d: %s\n", errno, strerror(errno));
        return -1;
    }

    /* Bind socket for server */
    if(bind(servSocket,(struct sockaddr *) &servAddr,sizeof(servAddr)) < 0)
    {
        printf("Failed to bind socket; Error %d: %s\n", errno, strerror(errno));
        return -1;
    }

    /* Setup socket for listening */
    if((listen(servSocket, maxConn)) < 0)
    {
        printf("Failed to listen on socket; Error %d: %s\n", errno, strerror(errno));
        return -1;
    }

    /* return the socket handle */
    return servSocket;

}

int main(int argc, char **argv)
{
    unsigned int portNum;
   	
  	if(argc != 2)
    {
        printf("Usage: bankServer portNum\n");
        return -1;
    }

	/* Setup TCP port number */
	portNum = atoi(argv[1]);







}
