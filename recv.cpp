#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "msg.h"    /* For the message struct */


/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr;

/* The name of the received file */
const char recvFileName[] = "recvfile";


/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{
	
	/* TODO: 1. Create a file called keyfile.txt containing string "Hello world" (you may do
 		    so manually or from the code).
	         2. Use ftok("keyfile.txt", 'a') in order to generate the key.
		 3. Use the key in the TODO's below. Use the same key for the queue
		    and the shared memory segment. This also serves to illustrate the difference
		    between the key and the id used in message queues and shared memory. The id
		    for any System V object (i.e. message queues, shared memory, and sempahores) 
		    is unique system-wide among all System V objects. Two objects, on the other hand,
		    may have the same key.
	 */
	key_t key;
	key= ftok("keyfile.txt",'a');
	/*-------------------------------------------------------------------------------------
	This function has 2 arguments. It uses the file name and the least significant 8 bits of
	proj_id (whic must be nonzero) to generate a key_t type System V IPC key, suitable for use
	with msgget(), semget(), or shmget().
	----------------------------------------------------------------------------------------*/
	//check for error of ftok()
	if(key==-1){
		perror("ftok error from receiver.cpp\n");
		exit(EXIT_FAILURE);
	}

	/* TODO: Allocate a piece of shared memory. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE. */
	shmid=shmget(key,SHARED_MEMORY_CHUNK_SIZE,0666);
	/*-------------------------------------------------------------------------------------
	This shmget() function is used to allocate a System V shared memory segment
	This function has 3 parameters: the key, the size of the segment, and the semflg.
	The semflg argument tell the shmget() which permission will be and specify if
	we want to create a new one or use the existing one. In this case we don't want to 
	create one, so no need to use IPC_CREAT, only need to specify 666 permission only.
	--------------------------------------------------------------------------------------*/
	//check for error of shmget()
	if(shmid==-1){
		perror("shmget error from receiver.cpp\n");
		exit(EXIT_FAILURE);
	}
	/* TODO: Attach to the shared memory */
	sharedMemPtr=shmat(shmid,(void *)0,0);
	/*----------------------------------------------------------------------------------------
	This shmat() is used to get the pointer from the shmid handle in order to
	attache to the shared memory. This function has 3 parameters: the 1st is the shared id we 
	from the call to shmget() above, the 2nd is shmaddr which is specific address to use but 
	we can just set it to 0 and let the OS choose the address for us, the 3rd parameter is 
	shmflg which can be set to SHM_RDONLY if you want to read from it, or set to 0 otherwise.
	-----------------------------------------------------------------------------------------*/
	//check for error of shmat()
	if(sharedMemPtr==(void *)(-1))
	{
		perror("shmat error from receiver.cpp\n");//shmat() return -1 on failure so
		//if it does we use perror to display a message error
		exit(EXIT_FAILURE);//and exit the program.
	}
	/* TODO: Create a message queue */
	/* Store the IDs and the pointer to the shared memory region in the corresponding parameters */
	msqid=msgget(key,0666|IPC_CREAT);
	/*-------------------------------------------------------------------------------------------
	This msgget() system call returns the message queue ID on success, or -1 on failure.This one
	will help to connect to the queue or create one if it doesn't exist.It has 2 parameters: 1st
	one is the key which is a system-wide unique identifier describing the queue we want to 
	connect or create, 2nd one is msgflg tells msgget() what to do. In this case we use 0666 for 
	permission -rw-rw-rw and IPC_CREAT to create a queue and connect to it.0666 -rw-rw-rw because 
	we are going to use msgrcv() and msgsnd()later and these function must have write and read 
	permission on the message queue in order to send a message, and to receive a message freely.
	-------------------------------------------------------------------------------------------*/
	//check for error of msgget()
	if(msqid==-1){
		perror("msgget error from receiver.cpp\n");
		exit(EXIT_FAILURE);
	}
}
 

/**
 * The main loop
 */
void mainLoop()
{
	/* The size of the mesage */
	int msgSize = 0;
	
	/* Open the file for writing */
	FILE* fp = fopen(recvFileName, "w");
		
	/* Error checks */
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

	/* Keep receiving until the sender set the size to 0, indicating that
 	 * there is no more data to send
 	 */	
	message sndMsg;
	message rcvMsg;
	msgSize=rcvMsg.size;//set the message size
	while(msgSize != 0)
	{	
		int valtemp=msgrcv(msqid,&rcvMsg,sizeof(message)-sizeof(long),SENDER_DATA_TYPE,0);
		 /*--------------------------------------------------------------------------------------------
		 This msgrcv() this will help us get the message out from the queue.This function has 5 arguments
		 the 1st,2nd and the 3rd are similar to the msgsnd() function in sender.cpp,the fourth one is type 
		 RECV_DONE_TYPE which is specified by professor.The last argument we just want to set it to 0
		 because this project does not require any special option for flag parameter.
		 --------------------------------------------------------------------------------------------*/
		//check for error of msgrcv()
		if(valtemp==-1){
			perror("msgrcv error in receiver.cpp");
			exit(1);
		}
		 else{
			 printf("Message received.\n");
		 }
		 //get the message size
		 msgSize=rcvMsg.size;
		/* If the sender is not telling us that we are done, then get to work */
		if(msgSize != 0)
		{
			/* Save the shared memory to file */
			if(fwrite(sharedMemPtr, sizeof(char), msgSize, fp) < 0)
			{
				perror("fwrite");
			}
			
			/* TODO: Tell the sender that we are ready for the next file chunk. 
 			 * I.e. send a message of type RECV_DONE_TYPE (the value of size field
 			 * does not matter in this case). 
 			 */
			 sndMsg.mtype=RECV_DONE_TYPE;
			int val3=msgsnd(msqid,&sndMsg,sizeof(message)-sizeof(long),0);
			/*-----------------------------------------------------------------------------------------
			This msgsnd() will pass information of message to a message queue. It has 4 arguments: 1st 
			is the msqid returned by msgget(), 2nd one is the msgp is a pointer to the data we want to 
			put on the queue, and it is a sndMsg in this case, 3rd is the msgsz is the size in bytes of 
			the data(message) to add to the queue(not count the size of the mtype member).That is why
			we have them subtract the sizeof(long) which is the size of mtype. The last argument is the 
			msgflag which allows us to set some option flag parameters, but in this project we don't need
			to, so we just set it to 0.
			-------------------------------------------------------------------------------------------*/
			//check for error of msgsnd()
			if(val3==-1){
				perror("msgsnd error from receiver.cpp\n");
				exit(EXIT_FAILURE);
			}
			else{
				printf("Message has sent.\n");
			}
		}
		/* We are done */
		else
		{
			/* Close the file */
			fclose(fp);
		}
	}
}



/**
 * Perfoms the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
	
	/* TODO: Deallocate the shared memory chunk */
	
	/* TODO: Deallocate the message queue */
}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */

void ctrlCSignal(int signal)
{
	/* Free system V resources */
	cleanUp(shmid, msqid, sharedMemPtr);
}

int main(int argc, char** argv)
{
	
	/* TODO: Install a singnal handler (see signaldemo.cpp sample file).
 	 * In a case user presses Ctrl-c your program should delete message
 	 * queues and shared memory before exiting. You may add the cleaning functionality
 	 * in ctrlCSignal().
 	 */
				
	/* Initialize */
	init(shmid, msqid, sharedMemPtr);
	
	/* Go to the main loop */
	mainLoop();

	/** TODO: Detach from shared memory segment, and deallocate shared memory and message queue (i.e. call cleanup) **/
		
	return 0;
}
