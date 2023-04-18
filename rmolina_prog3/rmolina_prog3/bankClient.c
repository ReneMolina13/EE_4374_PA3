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
	int clientStatus = *(parameter+1);
	
	// Initialize bank protocol structure
	sBANK_PROTOCOL randomRequest;
	randomRequest.trans = rand() % 3;
	randomRequest.acctnum = rand() % 20;
	randomRequest.value = rand();
	
	clientStatus = makeBankRequest(clientSocket, &randomRequest);
	*(parameter+1) = clientStatus;
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


int makeBankRequest(int clientSocket, sBANK_PROTOCOL *bankTransaction)
{
	// Send the requested transaction to the server
	
	ssize_t bytesSent;
	bytesSent = send(clientSocket, bankTransaction, sizeof(sBANK_PROTOCOL), 0);
	// Indicates transmission error
	if (bytesSent < 0)
		return -1;
	
	// Receive the response from the server
	ssize_t bytesReceived;
	bytesReceived = recv(clientSocket, bankTransaction, sizeof(sBANK_PROTOCOL), 0);
	// Indicates transmission error
	if (bytesReceived < 0)
		return -1;
	// Indicates that the server has closed the socket
	else if (bytesReceived == 0)
		return 0;
	
	return 1;
}


int makeThreads(int socket)
{
// TESTING
//**********************************************************************************
	puts("Creating a random number of threads to make random bank transactions");
//**********************************************************************************
	
	// Create between 0 and 100 threads to make random bank server requests
	srand(time(NULL));
	int numThreads = (rand() % 100) + 1;
	tid = (pthread_t *) malloc(numThreads * sizeof(pthread_t));
	pthread_attr_init(&attr);
	
// TESTING
//**********************************************************************************
	printf("%i threads will be created\n\n", numThreads);
	fputs("Thread Count: ", stdout);
//**********************************************************************************

	// Create the chosen number of threads
	int socketStatus[numThreads][2];
	for (int i = 0; i < numThreads; i++) {
		socketStatus[i][0] = socket;
		socketStatus[i][1] = 2;
		pthread_create(tid+i, &attr, serverThread, (void *) socketStatus[i]);
	}
	int threadStatus[numThreads];
	// Wait for all threads to terminate
	for (int i = 0; i < numThreads; i++) {
		// Pass a thread status to each thread (to act as a return value)
		pthread_join(*(tid + i), NULL);
		threadStatus[i] = socketStatus[i][1];
	}
	
	// Free array of tid structures and extract status values 
	free(tid);
	
	// Check if any threads were unsuccessful with their bank transactions
	bool transmissionError = false;
	bool socketClosed = false;
	for (int i = 0; i < numThreads; i++) {
		if (threadStatus[i] < 0) {
			transmissionError = true;
			fputs("Transmission error", stdout);
		}
		else if (threadStatus[i] == 0) {
			socketClosed = true;
			fputs("Socket closed", stdout);
		}
	}	
	
	// Return value depends on type of error (if any)
	if (transmissionError == true)
		return -1;
	else if (socketClosed == true)
		return 0;
	else {
		puts("\n\nAll threads have terminated successfully");
		return 1;
	}
}


bool newTransaction(NetInfo *sockData)
{
	// Ask if user wants to request another transaction
	printf("Would you like to make another transaction? (y/n) ");
	char c = getchar();
	if (c != 'y' && c != 'Y')
		return true;
	
	// Ask user for transaction type (to determine if value argument is needed)
	printf("\nEnter in new transaction for bank server\n");
	printf("Transaction (B = balance inquiry, D = deposit, W = withdraw): ");
	int numArgs;
	getchar();
	c = getchar();
	if (c == 'B' || c == 'b')
		numArgs = 6;
	else
		numArgs = 7;
	
	// Create argument array
	char **args = (char **) malloc(numArgs * sizeof(char *));
	for (int i = 0; i < numArgs-1; i++)	// args[numArgs-1] is NULL
		*(args+i) = calloc(20, sizeof(char));
	
	// Fill command line argument array with info from user
	
	int argsAssigned = 0;	// Keeps track of arguments successfully assigned
	int buffSize = 20;
	// Argument 0: Filename
	strncpy(args[0], "./bankClient", buffSize-1);
	argsAssigned++;
	// Argument 1: IP Address
	strncpy(args[1], sockData->cmdIP, buffSize-1);
	argsAssigned++;
	// Argument 2: Port Number
	snprintf(args[2], buffSize, "%u", sockData->cmdPort);
	argsAssigned++;
	// Argument 3: Transaction
	snprintf(args[3], buffSize, "%c", c);
	argsAssigned++;
	// Argument 4: Account Number
	printf("Account number: ");
	argsAssigned += scanf("%19s", args[4]);
	// Check if transaction value argument is needed
	if (numArgs == 7) {
		// Argument 5: Transaction Value
		printf("Value of the transaction in pennies: ");
		argsAssigned += scanf("%19s", args[5]);
		// Argument 6: End of arguments list (NULL)
		argsAssigned++;
	}
	else if (numArgs == 6) {
		// Argument 5: End of arguments list (NULL)
		argsAssigned++;
	}
	else {
		fputs("Error with argument array size - ", stderr);
		return false;
	}
	
// TESTING	
//**********************************************************************************
	printf("Arguments assigned: %i\n", argsAssigned);
	for (int i = 0; i < numArgs-1; i++)
		printf("Arg %i: %s\n", i, args[i]);
//**********************************************************************************
	
	// Check to make sure all arguments successfully assigned
	if (argsAssigned != numArgs) {
		fputs("Error assigning command line arguments - ", stderr);
		return false;
	}
	
	// Fork process & call this program from command line
	pid_t pid = fork();
	if (pid < 0) {
		fputs("Error forking process - ", stderr);
		return false;
	}
	// Child process: Arguments are new transaction specified by user
	else if (pid == 0) {
		
// TESTING	
//**********************************************************************************
		puts("Fork successful\n");
//**********************************************************************************		

		// execvp(args[0], args);
		execv(args[0], args);	
	}
	
	// Parent frees pointer memory before exiting
	for (int i = 0; i < numArgs-1; i++) {			
		free(*(args + i));
	}		
	free(args);
	
	return true;
}


int main(int argc, char **argv)
{	
// TESTING	
//**********************************************************************************
		puts("\n\nEntered main\n");
		for (int i = 0; i < argc; i++)
			printf("Argument %i: %s\n", i, argv[i]);
//**********************************************************************************

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
	
	// Status of bank transaction
	int status;
	
	// Make the transaction specified by the terminal arguments
	status = makeBankRequest(sockData.clientSocket, &mainRequest);
	if (status < 0) {
		fputs("Original transaction failed due to transmission error - ", stderr);
		return -1;
	}
	else if (status == 0) {
		puts("Socket in close-wait state: Initiating close handshake");
	}
	else {
		puts("Original transaction completed\n");

		// Create threads that make random transactions with bank server
		status = makeThreads(sockData.clientSocket);
		fputs("\n", stdout);
		if (status < 0) {
			fputs("Thread transaction(s) failed due to transmission error - ", stderr);
			return -1;
		}
		else if (status == 0)
			puts("Socket in close-wait state: Initiating close handshake");
		else
			puts("Bank transactions made by all threads were successful");
	}
	
	// Close client socket
	if (close(sockData.clientSocket) < 0) {
		fputs("Failed to properly close socket - ", stderr);
		return -1;
	}
	
// TESTING
//**********************************************************************************
	puts("Successfully closed socket\n");
	puts("Asking user for new transaction:");
//**********************************************************************************

	// Ask user for next bank server transaction
	if (newTransaction(&sockData) == false) {
		fputs("Unable to make requested transaction - ", stderr);
		return -1;
	}
	
// TESTING
//**********************************************************************************
	puts("Parent process terminating\n");
//**********************************************************************************	
	
	// End process without waiting for child
	return 0;
}
