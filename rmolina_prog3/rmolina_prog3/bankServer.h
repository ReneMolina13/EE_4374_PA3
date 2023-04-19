/*
	Rene Molina
	EE 4374 - Operating Systems
	Due Date: 4/12/2023
	Assigned: 3/27/2023
	
	This is the header file for the bank server program. It contains the structure 
	specified in the assignment instructions (sBANK_ACCT_DATA) containing the 
	account balance and a mutex semaphore for a single bank account. Also included 
	as a global variable is an array of bank accounts (sBANK_ACCT_DATA structures) 
	whose length is set by the macro NUM_ACCTS (100). Each of the functions used 
	in the bankServer program are described below.
*/


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
int handleClient(int clientSocket);
bool processTransaction();


// Array of bank accounts
sBANK_ACCT_DATA acctData[NUM_ACCTS];


#endif
