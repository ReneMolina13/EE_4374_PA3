/*
	Rene Molina
	EE 4374 - Operating Systems
	Due Date: 4/12/2023
	Assigned: 3/27/2023
	
	This is the header file for the bank client program. It contains a struct 
	called NetInfo which contains socket information obtained from the command 
	line arguments (cmdIP & cmdPort), the handle to the client socket, and 
	the Sockets API structure specifiying an internet address which is used to 
	connect with the bank server. Two global variables are also included to 
	enable the multithreaded portion of the code. The first is the structure 
	to give each thread default attributes, and the other is a pointer to 
	an arbitrary number of thread handles. Each of the functions used in the 
	bank client program are described below.
*/


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
bool newTransaction(NetInfo *);


// pthread attributes
pthread_attr_t attr;
// array of threads
pthread_t *tid;


#endif
