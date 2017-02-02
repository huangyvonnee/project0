#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "util.c"
#include "util.h"

const int MAX = 13;

static void doFib(int n, int doPrint);


/*
 * unix_error - unix-style error routine.
 */
// inline static void unix_error(char *msg)
// {
//     fprintf(stdout, "%s: %s\n", msg, strerror(errno));
//     exit(1);
// }


int main(int argc, char **argv)
{
    int arg;
    int print=1;

    if(argc != 2){
        fprintf(stderr, "Usage: fib <num>\n");
        exit(-1);
    }

    arg = atoi(argv[1]);
    if(arg < 0 || arg > MAX){
        fprintf(stderr, "number must be between 0 and %d\n", MAX);
        exit(-1);
    }

    doFib(arg, print);

    return 0;
}

/* 
 * Recursively compute the specified number. If print is
 * true, print it. Otherwise, provide it to my parent process.
 *
 * NOTE: The solution must be recursive and it must fork
 * a new child for each call. Each process should call
 * doFib() exactly once.
 */
static void doFib(int n, int doPrint)
{
    int status1;
    int status2;
    int fib;

    pid_t pid1 = Fork();
    if(pid1 == 0) {

        if(n <= 1)
            exit(n);
        else
            doFib(--n, 0);

    } else {

        pid_t pid2 = Fork();
        if(pid2 == 0) {

            if(n <= 1)
                exit(0);
            else
                doFib(n -= 2, 0);
            
        } else {

            if(waitpid(pid1, &status1, 0) > 0 && waitpid(pid2, &status2, 0) > 0)
                if(WIFEXITED(status1) && WIFEXITED(status2))
                    fib += WEXITSTATUS(status1) + WEXITSTATUS(status2);

         if(doPrint == 0)
                exit(fib);

        }
        
    }
    printf("%d\n", fib);
}