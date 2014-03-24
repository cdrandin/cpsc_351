#include <iostream>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include "msg.h"    /* For the message struct */

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msgid;

/* The pointer to the shared memory */
void* sharedMemPtr;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msgid - the id of the shared memory
 */

void init(int& shmid, int& msgid, void*& sharedMemPtr)
{
	/*  1. Create a file called keyfile.txt containing string "Hello world" (you may do
 		    so manually or from the code).
	    2. Use ftok("keyfile.txt", 'a') in order to generate the key.
		3. Use the key in the TODO's below. Use the same key for the queue
		    and the shared memory segment. This also serves to illustrate the difference
		    between the key and the id used in message queues and shared memory. The id
		    for any System V objest (i.e. message queues, shared memory, and sempahores) 
		    is unique system-wide among all SYstem V objects. Two objects, on the other hand,
		    may have the same key.
	 */
	/* make the key: */
	key_t key;
	if(key = ftok("/tmp", 'a') == (key_t) -1) 
	{
		perror("IPC error: ftok"); 
		exit(1);
	}

	/* Get the id of the shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE */
	/* connect to (and possibly create) the segment: */
	if((shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, IPC_CREAT)) == -1) 
	{
		perror("shmget");
		exit(1);
	}

	/* Attach to the shared memory */
    /* attach to the segment to get a pointer to it: */
	sharedMemPtr = shmat(shmid, (void *)0, 0);
	if(sharedMemPtr == (char *)(-1))
		perror("shmat");
	
	/* Create a message queue */
	if((msgid = msgget(key, IPC_CREAT)) == -1) 
	{
		perror("msgget");
		exit(1);
	}
	
	/* Store the IDs and the pointer to the shared memory region in the corresponding parameters */
}

/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msgid - the id of the message queue
 */
void cleanUp(const int& shmid, const int& msgid, void* sharedMemPtr)
{
	/* Detach from shared memory */
	/* detach from the segment: */ 	
	if(shmdt(sharedMemPtr) == -1)  	
	{ 		
		perror("shmdt"); 		
		exit(1); 	
	}  	

	/* deletes the shared memory segment */ 	
	/* 	
	if(shmctl(shmid, IPC_RMID, NULL) == -1) 	
	{ 	perror("shmctl"); 		
		exit(1); 	
	} 	
	*/
}

/**
 * The main send function
 * @param fileName - the name of the file
 */
void send(const char* fileName)
{
	/* Open the file for reading */
	FILE* fp = fopen(fileName, "r");

	/* A buffer to store message we will send to the receiver. */
	message sndMsg; 
	
	/* A buffer to store message received from the receiver. */
	message rcvMsg;
	
	/* Was the file open? */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}
	
	/* Read the whole file */
	while(!feof(fp))
	{
		/* Read at most SHARED_MEMORY_CHUNK_SIZE from the file and store them in shared memory. 
 		 * fread will return how many bytes it has actually read (since the last chunk may be less
 		 * than SHARED_MEMORY_CHUNK_SIZE).
 		 */
		if((sndMsg.size = fread(sharedMemPtr, sizeof(char), SHARED_MEMORY_CHUNK_SIZE, fp)) < 0)
		{
			perror("fread");
			exit(-1);
		}
	
		fscanf (fp, "%s", sndMsg.text);
		
		 /* ditch newline at end, if it exists */
        if (sndMsg.text[sndMsg.size-1] == '\n') 
        	sndMsg.text[sndMsg.size-1]  = '\0';

		/* Send a message to the receiver telling him that the data is ready 
 		 * (message of type SENDER_DATA_TYPE) 
 		 */
		sndMsg.mtype = SENDER_DATA_TYPE;

		std::cout << "Message is " << sndMsg.text << std::endl;
		std::cout << getpid() << " Entering Send process" << std::endl;

		//msgsnd(msgid, (void *) &sndMsg, sizeof(sndMsg.text), 0 /*IPC_NOWAIT*/);
		int send_val = msgsnd (msgid, &sndMsg.text, sizeof(sndMsg.text), IPC_NOWAIT);
		if(send_val == -1)
    		perror("Errror in send");

		//exit(1);
		/* TODO: Wait until the receiver sends us a message of type RECV_DONE_TYPE telling us 
 		 * that he finished saving the memory chunk. 
 		 */
		if(rcvMsg.mtype == RECV_DONE_TYPE)
			;
	}
	

	/** TODO: once we are out of the above loop, we have finished sending the file.
 	  * Lets tell the receiver that we have nothing more to send. We will do this by
 	  * sending a message of type SENDER_DATA_TYPE with size field set to 0. 	
	  */

		
	/* Close the file */
	fclose(fp);
	
}

int main(int argc, char** argv)
{
	
	/* Check the command line arguments */
	if(argc < 2)
	{
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}
		
	/* Connect to shared memory and the message queue */
	init(shmid, msgid, sharedMemPtr);
	
	/* Send the file */
	send(argv[1]);
	
	/* Cleanup */
	cleanUp(shmid, msgid, sharedMemPtr);
		
	return 0;
}
