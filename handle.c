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

 //Catches SIGINT and SIGUSR1 and outputs to STDOUT
 //according to specifications
 //Yvonne driving now
 void sigint_handler(int sig) {
	if(sig == SIGINT)
 		Write("Nice try.\n", 10);
 	else if(sig == SIGUSR1) {
 		Write("exiting\n", 10);
   	exit(1);
 	}
 } 

//Mohammad driving now
int main(int argc, char **argv)
{
	printf("Process ID: %d\n", getpid());
	
	signal(SIGINT, sigint_handler);
	signal(SIGUSR1, sigint_handler);

   struct timespec req, rem;
	while(1){
		//Resets timing data for increased accuracy
		//Manually setting to 0 ensures no garbage data
		req.tv_sec = 1;
		req.tv_nsec = 0;
		rem.tv_sec = 0;
		rem.tv_nsec = 0;
		
		printf("%s", "Still here\n");
		while(Nanosleep(&req, &rem) == -1){
			if(Nanosleep(&rem, &req) == 0)
				break;
		}
	}
	
  return 0;
}
