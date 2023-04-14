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

