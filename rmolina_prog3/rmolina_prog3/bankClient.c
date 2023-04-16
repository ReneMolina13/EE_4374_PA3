#include "bankClient.h"


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


bool parseCmdArgs(int argc, char **argv ,NetInfo *sockData, sBANK_PROTOCOL *mainRequest)
{
	// Check for correct number of arguments
	if (argc < 5 || argc > 6) {
		puts("Not enough arguments entered:");
		puts("1st argument should be IP address of the bank server");
		puts("2nd argument should be port number of the bank server");
		puts("3rd argument should be transaction: B = balance inquiry, D = deposit, W = withdraw");
		puts("4th argument should be the account number");
		puts("5th argument should be value of deposit or withdraw in pennies");
		return false;
	}
	
	// Extract command line arguemnts into appropriate structures
	sockData->cmdIP = *(argv + 1);
	sockData->cmdPort = atoi(*(argv + 2));
	switch(**(argv + 3)) {
	case 'B':
	case 'b':
		mainRequest->trans = BANK_TRANS_INQUIRY;
		mainRequest->acctnum = atoi(*(argv + 4));
		mainRequest->value = 0;
		break;
	case 'D':
	case 'd':
		mainRequest->trans = BANK_TRANS_DEPOSIT;
		mainRequest->acctnum = atoi(*(argv + 4));
		mainRequest->value = atoi(*(argv + 5));
		break;
	case 'W':
	case 'w':
		mainRequest->trans = BANK_TRANS_WITHDRAW;
		mainRequest->acctnum = atoi(*(argv + 4));
		mainRequest->value = atoi(*(argv + 5));
		break;
	default:
		puts("Invalid transaction");
		return false;
	}
	
	return true;
}


bool connectToServer(NetInfo *sockData)
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
	ssize_t bytesSent;
	bytesSent = send(clientSocket, bankTransaction, sizeof(sBANK_PROTOCOL), 0);
	if (bytesSent < 0) {
		puts("Unable to send request");
		return false;
	}
	else if (bytesSent == 0)
		puts("No data sent");
	else
		printf("Sent %li bytes out of a possible %lu\n", bytesSent, sizeof(sBANK_PROTOCOL));
	
	// Receive the response from the server
	ssize_t bytesReceived;
	bytesReceived = recv(clientSocket, bankTransaction, sizeof(sBANK_PROTOCOL), 0);
	if (bytesReceived < 0) {
		puts("Failed to get response from server");
		return false;
	}
	else if (bytesReceived == 0)
		puts("No data received");
	else
		printf("Received %li bytes out of a possible %lu\n", bytesReceived, sizeof(sBANK_PROTOCOL));
	
	return true;
}


void makeThreads(int socket)
{
// TESTING
//**********************************************************************************
	puts("Creating a random number threads to make random bank transactions\n");
//**********************************************************************************
	
	// Create between 0 and 100 threads to make random bank server requests
	srand(time(NULL));
	// int numThreads = (rand() % 100) + 1;
	int numThreads = 4;
	tid = malloc(numThreads * sizeof(pthread_t));
	pthread_attr_init(&attr);
	
	for (int i = 0; i < numThreads; i++)
		pthread_create(tid+i, &attr, serverThread, (void *) &socket);

// TESTING
//**********************************************************************************
	printf("All %i threads have been created\n\n", numThreads);
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


int main(int argc, char **argv)
{	
	// Input structures
	NetInfo sockData;	// Holds TCP Connection information
	sBANK_PROTOCOL mainRequest;	// Holds bank request info from user
	
	// Parse command line arguments
	if (parseCmdArgs(argc, argv, &sockData, &mainRequest) == false) {
		fputs("Unable to parse command line arguments - ", stderr);
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
	
	for (int iteration = 0; iteration < 10; iteration++) {
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
		puts("Original transaction completed\n");
//**********************************************************************************
		
		// Make threads to make random transactions with bank server
		// makeThreads(sockData.clientSocket);
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
