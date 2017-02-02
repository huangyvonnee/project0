#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "util.h"


/*
 * First, print out the process ID of this process.
 *
 * Then, set up the signal handler so that ^C causes
 * the program to print "Nice try.\n" and continue looping.
 *
 * Finally, loop forever, printing "Still here\n" once every
 * second.
 */
int main(int argc, char **argv)
{
	printf("\nProcess ID: %d\n", getpid());
	
	
	
   	struct timespec sec;
   	sec.tv_sec = 1;
	for(int i =0; i <= 5; i++){
		
		if(sigaction(SIGINT, SIG_IGN, NULL) == 0){
			ssize_t bytes; 
			const int STDOUT = 1; 
			bytes = write(STDOUT, "Nice try.\n", 10); 
			if(bytes != 10) 
   				exit(-999);
	}
		printf("%s", "Still here\n");
		nanosleep(&sec, NULL);
	}
	
  return 0;
}



