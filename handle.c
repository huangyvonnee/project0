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

//write method call and surrounding code taken from Project 0 Instructions
 void sigint_handler(int sig) {
 	ssize_t bytes;
 	const int STDOUT = 1;

 	if(sig == SIGINT) {
		bytes = write(STDOUT, " Nice try.\n", 10); 
		if(bytes != 10) 
   		exit(-999);
 	} else if(sig == SIGUSR1) {
		bytes = write(STDOUT, "exiting\n", 10); 
   		exit(1);
 	}
 }

int main(int argc, char **argv)
{
	printf("Process ID: %d\n", getpid());
	
	signal(SIGINT, sigint_handler);
	signal(SIGUSR1, sigint_handler);

   	struct timespec req, rem;
	while(1){
		req.tv_sec = 1;
		req.tv_nsec = 0;
		rem.tv_sec = 0;
		rem.tv_nsec = 0;
		
		printf("%s", "Still here\n");
		while(nanosleep(&req, &rem) == -1){
			if(nanosleep(&rem, &req) == 0)
				break;
		}
	}
	
  return 0;
}
