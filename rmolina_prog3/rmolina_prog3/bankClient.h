#ifndef	BANK_CLIENT_H
#define	BANK_CLIENT_H


#include "rmolina_banking.h"


// Structure holding connection info (from cmd line & necessary structures)
typedef struct
{
	char *cmdIP;
	unsigned int cmdPort;
	int clientSocket;
	struct sockaddr_in serverAddr;
} NetInfo;


// Function prototypes
void *serverThread(void *);
bool parseCmdArgs(int, char **, NetInfo *, sBANK_PROTOCOL *);
bool connectToServer(NetInfo *sockData);
int makeBankRequest(int, sBANK_PROTOCOL *);
int makeThreads(int);


// pthread attributes
pthread_attr_t attr;
// array of threads
pthread_t *tid;


#endif
