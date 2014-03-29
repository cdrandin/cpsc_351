#include <iostream>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "msg.h"    // For the message struct

// The size of the shared memory chunk
#define SHARED_MEMORY_CHUNK_SIZE 1024

int shmid, msgid;		// The ids for the shared memory segment and the message queue
void* sharedMemPtr;		// The pointer to the shared memory

// Sets up the shared memory segment and message queue
// @param shmid - the id of the allocated shared memory 
// @param msgid - the id of the shared memory
void init(int& shmid, int& msgid, void*& sharedMemPtr)
{
	// Make the key:
	key_t key = ftok("keyfile.txt", 'a');
	if(key < (key_t) 0) 
	{
		perror("IPC error: ftok"); 
		exit(-1);
	}

	// Get the id of the shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE
	// Connect to (and possibly create) the segment:
	shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, IPC_CREAT);
	if(shmid < 0) 
	{
		perror("shmget");
		exit(-1);
	}

	// Attach to the shared memory
    // attach to the segment to get a pointer to it:
	sharedMemPtr = shmat(shmid, (void *)0, 0);
	if(sharedMemPtr < (char *)(0))
	{
		perror("shmat");
		exit(-1);
	}
	// Create a message queue
	msgid = msgget(key, 0666);
	if(msgid < 0) 
	{
		perror("msgget");
		exit(-1);
	}
	
	// Store the IDs and the pointer to the shared memory region in the corresponding parameters
}


// Performs the cleanup functions
// @param sharedMemPtr - the pointer to the shared memory
// @param shmid - the id of the shared memory segment
// @param msgid - the id of the message queue
void cleanUp(const int& shmid, const int& msgid, void* sharedMemPtr)
{
	// Detach from shared memory
	if(shmdt(sharedMemPtr) < 0)
		perror("shmdt");
}

// The main send function
// @param fileName - the name of the file
void send(const char* fileName)
{
	// Open the file for reading
	FILE* fp = fopen(fileName, "r");

	// A buffer to store message we will send to the receiver.
	message sndMsg; 
	sndMsg.mtype = SENDER_DATA_TYPE; 
	message rcvMsg;			// A buffer to store message received from the receiver.
	
	// Was the file open?
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}
	
	// Read the whole file
	while(sndMsg.size != 0)
	{
		// Read at most SHARED_MEMORY_CHUNK_SIZE from the file and store them in shared memory. 
 		// fread will return how many bytes it has actually read (since the last chunk may be less
 		// than SHARED_MEMORY_CHUNK_SIZE).
		if((sndMsg.size = fread(sharedMemPtr, sizeof(char), SHARED_MEMORY_CHUNK_SIZE, fp)) < 0)
		{
			perror("fread");
			exit(-1);
		}
	
		if(msgsnd(msgid, &sndMsg, sizeof(sndMsg) - sizeof(long), 0) < 0)
		{
    		perror("msgsnd failed");
    		exit(-1);
    	}
    	
    	if(sndMsg.size != 0)
		{
			if(msgrcv(msgid, &rcvMsg, sizeof(rcvMsg) - sizeof(long),RECV_DONE_TYPE,0) < 0) 
			{
				perror("msgrcv");
				exit(-1);
			}
		}
	}
	
 	if(msgsnd(msgid, &sndMsg, sizeof(sndMsg) - sizeof(long),0) < 0) 
	{
		perror("msgsend");
		exit(-1);
	}

 	// Destroy message queue connection
	if (msgctl(msgid, IPC_RMID, NULL) == -1) 
	{
		perror("msgctl");
		exit(1);
	} 
	
	// Close the file
	fclose(fp);
	free(sharedMemPtr);
}

int main(int argc, char** argv)
{
	
	// Check the command line arguments
	if(argc < 2)
	{
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}
		
	// Connect to shared memory and the message queue
	init(shmid, msgid, sharedMemPtr);
	
	// Send the file
	send(argv[1]);

	// Cleanup
	cleanUp(shmid, msgid, sharedMemPtr);
		
	return 0;
}