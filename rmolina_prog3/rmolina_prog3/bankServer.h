#ifndef	BANK_SERVER_H
#define BANK_SERVER_H


#include "rmolina_banking.h"


// Number of bank accounts
#define NUM_ACCTS 100


// Server-side banking information
typedef struct
{
	unsigned int balance;
	pthread_mutex_t mutex;
} sBANK_ACCT_DATA;


// Function Prototypes
int initBank(struct sockaddr_in *);
bool handleClient(int clientSocket);
bool processTransaction();


// Array of bank accounts
sBANK_ACCT_DATA acctData[NUM_ACCTS];


#endif
