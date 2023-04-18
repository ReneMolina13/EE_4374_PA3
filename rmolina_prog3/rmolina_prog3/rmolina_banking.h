#ifndef RMOLINA_BANKING_H
#define RMOLINA_BANKING_H


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


// Bank Transaction Types
#define BANK_TRANS_DEPOSIT 0
#define BANK_TRANS_WITHDRAW 1
#define BANK_TRANS_INQUIRY 2


// Client-side banking protocol
typedef struct
{
	unsigned int trans; /* transaction type */
	unsigned int acctnum; /* account number */
	unsigned int value; /* value */
} sBANK_PROTOCOL;


#endif
