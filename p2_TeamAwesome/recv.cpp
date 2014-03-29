#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "msg.h"    // For the message struct

// The size of the shared memory chunk
#define SHARED_MEMORY_CHUNK_SIZE 1024

int shmid, msqid;							// The ids for the shared memory segment and the message queue
void *sharedMemPtr;							// The pointer to the shared memory
const char recvFileName[] = "recvfile";		// The name of the received file

 // Sets up the shared memory segment and message queue
 // @param shmid - the id of the allocated shared memory 
 // @param msqid - the id of the shared memory
 // @param sharedMemPtr - the pointer to the shared memory
void init(int& shmid, int& msqid, void*& sharedMemPtr)
{
	// Generate key
	key_t key = ftok("keyfile.txt", 'a');
	if(key == -1) 
	{
        perror("ftok");
        exit(1);
    }
	
	// Allocate a piece of shared memory. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE.
	// shmid: shared memory ID
    if((shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, IPC_CREAT | 0666)) == -1) 
    {
        perror("shmget");
        exit(1);
    }

	// Attach to the shared memory
	sharedMemPtr = shmat(shmid, (void*)0 , 0); // let OS choose address; read and write
	if(sharedMemPtr == (char*)(-1))
	{
		perror("shmat");
		exit(1);
	}

	// Create a message queue
	msqid = msgget(key, IPC_CREAT | 0666);
	if(msqid == -1)
	{
		perror("msqid");
		exit(1);
	}
}

// The main loop
void mainLoop()
{
	int msgSize = 0;							// The size of the mesage
	FILE* fp = fopen(recvFileName, "w");		// Open the file for writing
		
	// Error checks
	if(!fp)
	{
		perror("fopen");	
		exit(-1);
	}

	// Keep receiving until the sender set the size to 0, indicating that
 	//   there is no more data to send	
	// Declare messages
	message rcvMsg;
	rcvMsg.size = 0;
	message sndMsg;
	sndMsg.mtype = RECV_DONE_TYPE;
	sndMsg.size = 1;
	do
	{	
		if(msgrcv (msqid, &rcvMsg, sizeof(message) - sizeof(long),SENDER_DATA_TYPE,0) < 0)
		{
			perror("msgrcv");
			exit(-1);
		}
		else
			msgSize = rcvMsg.size;

		// If the sender is not telling us that we are done, then get to work
		// Save the shared memory to file
		if(msgSize > 0)
		{
			if(fwrite(sharedMemPtr, sizeof(char), msgSize, fp) < 0)
				perror("fwrite");
			if(msgsnd(msqid, &sndMsg, sizeof(message) - sizeof(long), 0) < 0)
			{
				perror("msgsnd2");
				exit(-1);
			}
		}	
	} while(msgSize != 0);
}


// Perfoms the cleanup functions
// @param sharedMemPtr - the pointer to the shared memory
// @param shmid - the id of the shared memory segment
// @param msqid - the id of the message queue
void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	// Detach from shared memory
    if (shmdt(sharedMemPtr) == -1)
    { 	// detach
        perror("shmdt");
        exit(1);
    }

   	// Deallocate the shared memory chunk	
    if (shmctl(shmid, IPC_RMID, NULL) < 0)
    { 	// remove
    	perror("shmctl");
    	exit(1);
    }
	
	// Deallocate the message queue
	if (msgctl(msqid, IPC_RMID, NULL) < 0)
	{
		exit(1);
	}

}

// Handles the exit signal
// @param signal - the signal type
void ctrlCSignal(int signal)
{
	// Free system resources
	cleanUp(shmid, msqid, sharedMemPtr);
}

int main(int argc, char** argv)
{
	// struct message buffer;
	// Calls the cleanUp() function when the user presses Ctrl-C
	signal(SIGINT, ctrlCSignal);
				
	// Initialize
	init(shmid, msqid, sharedMemPtr);
	
	// Go to the main loop
	mainLoop();

	// Detach from shared memory segment, and deallocate shared memory and message queue (i.e. call cleanup)
	cleanUp(shmid, msqid, sharedMemPtr);
	
	return 0;
}
