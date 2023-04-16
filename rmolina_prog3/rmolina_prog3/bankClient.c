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

// Structure holding connection info (from cmd line & necessary structures)
typedef struct
{
	char *cmdIP;
	unsigned int cmdPort;
	int clientSocket;
	struct sockaddr_in serverAddr;
} connectionInfo;

// Function prototypes
void *serverThread(void *param);
bool connectToServer(connectionInfo *sockData);
bool makeBankRequest(int, sBANK_PROTOCOL *bankTransaction);

// pthread attributes
pthread_attr_t attr;
// array of threads
pthread_t *tid;

// TESTING
//**********************************************************************************
int activeThreads = 0;
//**********************************************************************************


void *serverThread(void *param)
{
// TESTING
//**********************************************************************************
	printf("%i ", ++activeThreads);
//**********************************************************************************

	int *parameter = (int *) param;
	int clientSocket = *parameter;
	
	// Initialize bank protocol structure
	sBANK_PROTOCOL randomRequest;
	randomRequest.trans = rand() % 3;
	randomRequest.acctnum = rand() % 100;
	randomRequest.value = rand();
	
	makeBankRequest(clientSocket, &randomRequest);
	pthread_exit(0);
}


bool connectToServer(connectionInfo *sockData)
{
	// Create TCP client socket
	sockData->clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockData->clientSocket < 0) {
		puts("Error creating socket");
		return false;
	}
	
	// Initialize structure for address
	memset(&sockData->serverAddr, 0, sizeof(sockData->serverAddr));
	sockData->serverAddr.sin_family = AF_INET;
	sockData->serverAddr.sin_addr.s_addr = inet_addr(sockData->cmdIP);
	sockData->serverAddr.sin_port = htons(sockData->cmdPort);
	
	// Connect to server
	if (connect(sockData->clientSocket, (struct sockaddr *) &(sockData->serverAddr), sizeof(struct sockaddr)) < 0) {
		puts("Unable to connect to server");
		return false;
	}
	
	return true;
}


bool makeBankRequest(int clientSocket, sBANK_PROTOCOL *bankTransaction)
{
	// Send the requested transaction to the server
	if (send(clientSocket, bankTransaction, sizeof(sBANK_PROTOCOL), 0) < 0) {
		puts("Unable to send request");
		return false;
	}
	
	// Receive the response from the server
	if (recv(clientSocket, bankTransaction, sizeof(sBANK_PROTOCOL), 0) < 0) {
		puts("Failed to get response from server");
		return false;
	}
	
	return true;
}


int main(int argc, char **argv)
{	
	// Check for correct number of arguments
	if (argc < 5 && argc > 6) {
		fputs("Not enough arguments entered:\n", stderr);
		fputs("1st argument should be IP address of the bank server\n", stderr);
		fputs("2nd argument should be port number of the bank server\n", stderr);
		fputs("3rd argument should be transaction: ", stderr);
		fputs("B = balance inquiry, D = deposit, W = withdraw\n", stderr);
		fputs("4th argument should be the account number\n", stderr);
		fputs("5th argument should be value of deposit or withdraw in pennies\n", stderr);
		return -1;
	}
	
	// Extract command line arguemnts into appropriate structures
	connectionInfo sockData;
	sockData.cmdIP = *(argv + 1);
	sockData.cmdPort = atoi(*(argv + 2));
	sBANK_PROTOCOL mainRequest;
	switch(**(argv + 3)) {
	case 'B':
	case 'b':
		mainRequest.trans = BANK_TRANS_INQUIRY;
		mainRequest.acctnum = atoi(*(argv + 4));
		mainRequest.value = 0;
		break;
	case 'D':
	case 'd':
		mainRequest.trans = BANK_TRANS_DEPOSIT;
		mainRequest.acctnum = atoi(*(argv + 4));
		mainRequest.value = atoi(*(argv + 5));
		break;
	case 'W':
	case 'w':
		mainRequest.trans = BANK_TRANS_WITHDRAW;
		mainRequest.acctnum = atoi(*(argv + 4));
		mainRequest.value = atoi(*(argv + 5));
		break;
	default:
		fputs("Invalid transaction - ", stderr);
		return -1;
	}
	
	// Connect to bank server
	if (connectToServer(&sockData) == false) {
		fputs("Unable to connect to bank server - ", stderr);
		return -1;
	}
	
// TESTING
//**********************************************************************************
	puts("\nConnected to bank server: ");
	puts("Making original transaction\n");
//**********************************************************************************
	
	for (int iteration = 10; i > 0; i++) {
		// TESTING
//**********************************************************************************
		printf("Iteration number: %i\n", iteration);
//**********************************************************************************
		
		// Make the transaction specified by the terminal arguments
		if (makeBankRequest(sockData.clientSocket, &mainRequest) == false) {
			fputs("Unable to make original transaction (from terminal arguments) - ", stderr);
			return -1;
		}
		
// TESTING
//**********************************************************************************
		puts("Original transaction completed: ");
//**********************************************************************************		

		// Create between 0 and 100 threads to make random bank server requests
		srand(time(NULL));
		// int numThreads = (rand() % 100) + 1;
		int numThreads = 0;
		tid = malloc(numThreads * sizeof(pthread_t));
		pthread_attr_init(&attr);
		
// TESTING
//**********************************************************************************
		printf("Creating %i threads to make bank transactions\n", numThreads);
//**********************************************************************************
		
		for (int i = 0; i < numThreads; i++)
			pthread_create(tid+i, &attr, serverThread, (void *) &(sockData.clientSocket));

// TESTING
//**********************************************************************************
		puts("All threads have been created\n");
//**********************************************************************************


		// Wait for all threads to terminate
		for (int i = 0; i < numThreads; i++)
			pthread_join(*(tid+i), NULL);
		free(tid);
		
// TESTING
//**********************************************************************************
		puts("\n\nAll threads have terminated\n");
//**********************************************************************************
	}
	
	// Close client socket
	if (close(sockData.clientSocket) < 0) {
		fputs("Failed to properly close client socket - ", stderr);
		return -1;
	}
	
// TESTING
//**********************************************************************************
	puts("Client socket closed:");
//**********************************************************************************
	
	// End process without waiting for child
	return 0;
}