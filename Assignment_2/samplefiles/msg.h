// The information type

#define SENDER_DATA_TYPE 1

// The done message
#define RECV_DONE_TYPE 2

/**
 * The message structure
 */

struct message
{
	// The message type
	long mtype;
	
	// How many bytes in the message
	int size;

	// Data that will be added to the queue
	//char mtext[1];
	
	// Send message across
	char text[20];
	
	/**
 	 * Prints the structure
 	 * @param fp - the file stream to print to
 	 */
	void print(FILE* fp)
	{
		fprintf(fp, "%d %d", (int)mtype, size);
	}
};
