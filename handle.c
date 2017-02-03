#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "util.h"

void sigHand() {
	ssize_t bytes; 
	const int STDOUT = 1; 
	bytes = write(STDOUT, "Nice try.\n", 10); 
	if(bytes != 10) 
   	exit(-999);
}

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
	signal(SIGINT, sigHand);
   struct timespec req, rem;
	while(1) {
		req.tv_sec = 1;
		req.tv_nsec = 0;
		rem.tv_sec = 0;
		rem.tv_nsec = 0;
		printf("%s", "Still here\n");
		while(nanosleep(&req, &rem) == -1) {
			if(nanosleep(&rem, &req) == 0)
				break;
		}
	}
	
  return 0;
}
