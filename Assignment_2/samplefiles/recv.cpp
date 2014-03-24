#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include "msg.h"    // For the message struct

// The size of the shared memory chunk
#define SHARED_MEMORY_CHUNK_SIZE 1024

using namespace std;

// The ids for the shared memory segment and the message queue
int shmid, msqid;
int errorcheck;

// The pointer to the shared memory
void *sharedMemPtr;

// The name of the received file
const char recvFileName[] = "recvfile";

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */
void init(int& shmid, int& msqid, void*& sharedMemPtr)
{
	/* 1. Create a file called keyfile.txt containing string "Hello world" (you may do
		    so manually or from the code).
     2. Use ftok("keyfile.txt", 'a') in order to generate the key.
	 3. Use the key in the TODO's below. Use the same key for the queue
	    and the shared memory segment. This also serves to illustrate the difference
	    between the key and the id used in message queues and shared memory. The id
	    for any System V object (i.e. message queues, shared memory, and sempahores) 
	    is unique system-wide among all System V objects. Two objects, on the other hand,
	    may have the same key.
	*/
	// generate key
	key_t key = ftok("keyfile.txt", 'a');
	if (key == -1) {
        perror("ftok");
        exit(1);
    }
	
	// Allocate a piece of shared memory. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE.
	// shmid: shared memory ID
    if ((shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

	// Attach to the shared memory
	sharedMemPtr = shmat(shmid, (void*)0 , 0); // let OS choose address; read and write
	if(sharedMemPtr == (char*)(-1)){
		perror("shmat");
		exit(1);
	}

	// Create a message queue
	msqid = msgget(key, IPC_CREAT);
	if(msqid == -1)
		perror("msqid");
}
 

/**
 * The main loop
 */
void mainLoop()
{
	// The size of the mesage
	int msgSize = 0;

	// Store message received from the sender.
	message recvMessage;
	
	// Open the file for writing
	FILE* fp = fopen(recvFileName, "w");
		
	// Error checks
	if(!fp)
	{
		perror("fopen");	
		exit(-1);
	}

    /* TODO: Receive the message and get the message size. The message will 
   	* contain regular information. The message will be of SENDER_DATA_TYPE
    * (the macro SENDER_DATA_TYPE is defined in msg.h).  If the size field
    * of the message is not 0, then we copy that many bytes from the shared
    * memory region to the file. Otherwise, if 0, then we close the file and
    * exit.
    *
    * NOTE: the received file will always be saved into the file called
    * "recvfile"
    */

	// Keep receiving until the sender set the size to 0, indicating that
 	// there is no more data to send	
	do {	

		puts("here1");
		// The message is received
		if (msgrcv(msqid, (void *) &recvMessage, sizeof(recvMessage.size), 0, 0 /*IPC_NOWAIT*/) == -1) {
			perror("msgrcv");
			puts("here2");
			exit(-1);
		}
		puts("here");
		msgSize = recvMessage.size;
		std::cout << msgSize << std::endl;
		// If the sender is not telling us that we are done, then get to work */
		if(msgSize != 0)
		{
			// Save the shared memory to file
			if(fwrite(sharedMemPtr, sizeof(char), msgSize, fp) < 0)
			{
				perror("fwrite");
			}
			
			/* TODO: Tell the sender that we are ready for the next file chunk. 
 			 * I.e. send a message of type RECV_DONE_TYPE (the value of size field
 			 * does not matter in this case). 
 			 */
			recvMessage.mtype = RECV_DONE_TYPE;
			if(msgsnd (msqid, &recvMessage.text, strlen(recvMessage.text)+1, 0) == -1)
			{
	    		perror("Error in send");
	    		exit(-1);
	    	}
			
			
		}
		// We are done
		else
		{
			// Close the file
			fclose(fp);
		}

	} while (msgSize != 0);
}


/**
 * Perfoms the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */
void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	// Detach from shared memory
    if (shmdt(sharedMemPtr) == -1) { // detach
        perror("shmdt");
        exit(1);
    }

   	// Deallocate the shared memory chunk	
    if (shmctl(shmid, IPC_RMID, NULL) == -1) { // remove
    	perror("shmctl");
    	exit(1);
    }
	
	// Deallocate the message queue
	if (msgctl(msqid, IPC_RMID, NULL) == -1) {
		perror("msgctl");
		exit(1);
	}

}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */
void ctrlCSignal(int signal)
{
	// Free system V resources
	cout << "\nMark's the best" << endl;
	cleanUp(shmid, msqid, sharedMemPtr);
}

int main(int argc, char** argv)
{
	//struct message buffer;
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
