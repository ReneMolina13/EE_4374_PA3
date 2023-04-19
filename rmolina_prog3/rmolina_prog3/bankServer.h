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
	in the bankServer program are described below
*/


#ifndef	BANK_SERVER_H
#define BANK_SERVER_H


#include "rmolina_banking.h"


// Number of bank accounts
#define NUM_ACCTS 100
// Error codes for threads
#define ACCEPT_ERROR		-1
#define TRANSMISSION_ERROR	-2
#define CLOSE_ERROR			-3


// Server-side banking information
typedef struct
{
	unsigned int balance;
	pthread_mutex_t mutex;
} sBANK_ACCT_DATA;

// Structure holding thread arguments
typedef struct
{
	pthread_t tid;
	int serverSocket;
	int errorCode;
} ThreadArgs;


//*******************************************************************************************
// Function Prototypes
//*******************************************************************************************


void *clientThread(void *);

/*
	initBank function: 
	
	Description:
	Initializes the bank accounts to random values, creates the TCP socket for the bank 
	server, initializes the sockaddr_in structure from the Sockets API, binds the TCP
	socket to port 26207, and sets it to be listening to incoming connections
	
	Inputs:
	- struct sockaddr_in *serverAddr: A pointer to an empty sockaddr_in structure
	
	Outputs:
	- int serverSocket: The handle for the server socket
	- struct sockaddr_in *serverAddr: By the time the function returns, this structure has been
			filled with the connection info for the bank server
*/
int initBank(struct sockaddr_in *);

/*
	handleClient function:
	
	Description:
	Handles communication with the client. Waits to receive the transaction request from 
	the connected client, calls a function to process the transaction, and sends the receipt 
	of the transaction back to the client
	
	Inputs:
	- int clientSocket: The handle for the connected socket between the bank server and a
			particular client. Used to send and receive transaction request and send
			transaction receipts
	
	Outputs:
	- int status: Indicates if the transaction was successful. Value is -1 if there was an
			error sending or receiving data on the socket. Value is 0 if the client closed
			the connected socket. Value is 1 if the transaction was successful
*/
int handleClient(int clientSocket);

/*
	processTransaction function:
	
	Description:
	Processes the requested transaction received by the server
	
	Inputs:
	- sBANK_PROTOCOL *request: A pointer to the sBANK_PROTOCOL structure received by the
			bank server, representing the client's requested transaction
		
	Outputs:
	- bool success: A flag signaling if the transaction was successful or not. Its value is
			false if the provided account number is invalid, the requested transaction is 
			invalid, or if more money is asked to be withdrawn than is available in the 
			provided bank account. Its value is true otherwise
	- sBANK_PROTOCOL *request: The receipt of the transaction is stored back in this structure
			to be sent back to the client
*/
bool processTransaction();


// Array of bank accounts
sBANK_ACCT_DATA acctData[NUM_ACCTS];

// Attribute structure for threads
pthread_attr_t attr;

// Array of thread structures
ThreadArgs args[NUM_ACCTS];

#endif
