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

 void sigint_handler(int sig)
 {
 	// taken from Project 0 Instructions
 	ssize_t bytes; 
	const int STDOUT = 1; 
	bytes = write(STDOUT, "Nice try.\n", 10); 
	if(bytes != 10) 
   	exit(-999);
 }
 
 void sigint_handler2(int sig)
 {
 	// taken from Project 0 Instructions
 	ssize_t bytes; 
	const int STDOUT = 1; 
	bytes = write(STDOUT, "exiting\n", 10); 
   	exit(1);
 }

int main(int argc, char **argv)
{
	printf("\nProcess ID: %d\n", getpid());
	
	signal(SIGINT, sigint_handler);
	signal(SIGUSR1, sigint_handler2);

   	struct timespec sec, nsec;
	while(1){
		sec.tv_sec = 1;
		
		printf("%s", "Still here\n");
		while(nanosleep(&sec, &nsec) == -1){
			if(nanosleep(&nsec, &sec) == 0)
				break;
		}
	}
	
  return 0;
}


