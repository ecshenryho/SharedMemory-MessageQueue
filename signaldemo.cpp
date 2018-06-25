
/** This program illustrates a simple signal handler 
 * for the case where the user presses Ctrl-C. Test
 * it by running it and pressing Ctrl-C.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* The counter which counts the number 
 * of times the user pressed Ctrl-C
 */
int count = 9;

/**
 * This function handles the singnal
 * @param arg - the signal number
 */

void signalHandlerFunc(int arg)
{
	/* We can take more Ctrl-Cs */
	if(count > 0)
		fprintf(stderr, "Haha I have %d lives!\n", count);
	else
	{
		fprintf(stderr, "Ahh you got me!\n");
		exit(0);
	}
	
	--count;
}


int main()
{
	/* Overide the default signal handler for the
	 * SIGINT signal with signalHandlerFunc
	 */
	signal(SIGINT, signalHandlerFunc); 
	
	/* Spin the loop forever */
	for(;;)
	{
		sleep(1);
	}
	
	return 0;
}

