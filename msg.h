/* The information type */ 

#define SENDER_DATA_TYPE 1

/* The done message */
#define RECV_DONE_TYPE 2

/**
 * The message structure
 */


struct message
{
	/* The message type */
	long mtype;
	
	/* How many bytes in the message */
	int size;
	
	/**
 	 * Prints the structure
 	 * @param fp - the file stream to print to
 	 */

	void print(FILE* fp)
	{
		fprintf(fp, "%d %d", mtype, size);
	}
};
