#include <stdio.h>		// Standard I/O library
#include <stdlib.h>		// Standard library: atoi(), malloc
#include <pthread.h>	// pthreads library
#include <unistd.h>		// Unix system call library
#include <signal.h>		// Signal handler library
#include <time.h>		// srand function
#include <string.h>		// memset function
#include <stdbool.h>	// bool data type
#include <sys/socket.h>	
#include <arpa/inet.h>
#include <netdb.h>
#include "rmolina_banking.h"

// Number of bank accounts
#define NUM_ACCTS 100

// Server-side banking information
typedef struct
{
	unsigned int balance;
	pthread_mutex_t mutex;
} sBANK_ACCT_DATA;

// Array of bank accounts
sBANK_ACCT_DATA acctData[NUM_ACCTS];

// Processes transction requested by client
bool processTransaction(sBANK_PROTOCOL *request)
{	
	// Checks for a valid account number
	if (request->acctnum < 0 || request->acctnum >= NUM_ACCTS) {
		puts("Invalid account number");
		return false;
	}
	
	// Use mutex to prevent race conditions
	pthread_mutex_lock(&acctData[request->acctnum].mutex);

	// Check for valid request
	bool success = true;
	switch(request->trans) {
	
	// Deposit
	case BANK_TRANS_DEPOSIT:
		acctData[request->acctnum].balance += request->value;
		break;
	
	// Withdraw
	case BANK_TRANS_WITHDRAW:
		// Check for sufficient funds
		if (acctData[request->acctnum].balance < request->value) {
			puts("Insufficient Funds");
			request->value = 0;
			success = false;
		}
		else 
			acctData[request->acctnum].balance -= request->value;
		break;
		
	// Show account balance
	case BANK_TRANS_INQUIRY:
		request->value = acctData[request->acctnum].balance;
		break;
		
	default:
		puts("Invalid transaction");
		success = false;
	}
	
	// Unlock mutex and return whether transaction was successful
	pthread_mutex_unlock(&acctData[request->acctnum].mutex);
	return success;
}

int main()
{
	// Initialize bank
	srand(time(NULL));
	for (int i = 0; i < NUM_ACCTS; i++) {
		// Set account balances to random values
		acctData[i].balance = rand();
		// Initialize account mutexes
		pthread_mutex_init(&acctData[i].mutex, NULL);
	}
	
	
	// Create TCP server socket
	int serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket < 0) {
		fputs("Error creating socket - ", stderr);
		return -1;
	}

	// Initialize address structure
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// Allows connection to any IP address
	serverAddr.sin_port = htons(26207);
	
	
	
	// TESTING
	puts("TCP socket created:");
	printf("Socket value: %i\n\n", serverSocket);
	
	
	
	// Bind local address to socket
	if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr)) < 0) {
		fputs("Error binding local address to socket - ", stderr);
		return -1;
	}
	
	
	
	// TESTING
	puts("TCP socket bound to address");
	printf("Server family value: %i\n", serverAddr.sin_family);
	printf("Server IP value: %i\n", serverAddr.sin_addr.s_addr);
	printf("Server port value: %i\n\n", ntohs(serverAddr.sin_port));
	
	
	
	// Have server listen for bank customers
	if (listen(serverSocket, NUM_ACCTS) < 0) {
		fputs("Server unable to listen for traffic - ", stderr);
		return -1;
	}
	
	
	
	// TESTING
	puts("Server is now listening for incoming connections\n");
	
	
	
	// Run forever
	while (1) {
		// Accept client connection
		char clientName[INET_ADDRSTRLEN];
		struct sockaddr_in clientAddr;
		socklen_t clientAddrLength = sizeof(clientAddr);
		int clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &clientAddrLength);
		if (clientSocket < 0) {
			puts("Unable to accept client connection");
			break;
		}
		inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientName, sizeof(clientName));
			
		
		
		
		// TESTING
		puts("Server accepted connection request:");
		printf("Client socket value: %i\n", clientSocket);
		printf("Client family value: %i\n", clientAddr.sin_family);
		printf("Client IP value: %s\n", clientName);
		printf("Client port value: %i\n\n", ntohs(clientAddr.sin_port));
		
		
		
		// Receive request from client
		sBANK_PROTOCOL clientRequest;
		if (recv(clientSocket, &clientRequest, sizeof(sBANK_PROTOCOL), 0) < 0) {
			puts("Unable to receive request from client");
			break;
		}
		
		
		
		// TESTING
		puts("Received request from client");
		printf("Transaction type (D=0, W=1, I=2): %i\n", clientRequest.trans);
		printf("Account number: %i\n", clientRequest.acctnum);
		printf("Value of transaction: %i\n\n", clientRequest.value);
		
		
		
		// Perform requested transaction 
		if (processTransaction(&clientRequest) == false)
			puts("Unable to complete transaction");
		
		
		
		// TESTING
		puts("Transaction Completed");
		puts("Receipt for client: ");
		printf("Transaction type (D=0, W=1, I=2): %i\n", clientRequest.trans);
		printf("Account number: %i\n", clientRequest.acctnum);
		printf("Value of transaction: %i\n\n", clientRequest.value);
		
		
		
		// Confirm with client that request was completed
		if (send(serverSocket, &clientRequest, sizeof(sBANK_PROTOCOL), 0) < 0) {
			puts("Unable to confirm completion of request to client");
			break;
		}
		
		
		
		// TESTING
		puts("Receipt received by client\n");
		
		
		
	}
		
	// Close server socket
	if (close(serverSocket) < 0) {
		fputs("Unable to properly close server socket - ", stderr);
		return -1;
	}
	
	// Exit program
	return 0;
}
