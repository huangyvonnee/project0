#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include "util.h"

/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.  
 */
int parseline(const char *cmdline, char **argv) 
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
    buf++;

    /* Build the argv list */
    argc = 0;
    if (*buf == '\'') {
    buf++;
    delim = strchr(buf, '\'');
    }
    else {
    delim = strchr(buf, ' ');
    }

    while (delim) {
    argv[argc++] = buf;
    *delim = '\0';
    buf = delim + 1;
    while (*buf && (*buf == ' ')) /* ignore spaces */
           buf++;

    if (*buf == '\'') {
        buf++;
        delim = strchr(buf, '\'');
    }
    else {
        delim = strchr(buf, ' ');
    }
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* ignore blank line */
    return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
    argv[--argc] = NULL;
    }
    return bg;
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
    unix_error("Signal error");
    return (old_action.sa_handler);
}

//From string notes posted by Norman
void substr(char dest[], char src[], int offset, int len)
{
int i;
for(i = 0; i < len && src[offset + i] != '\0'; i++)
    dest[i] = src[i + offset];
dest[i] = '\0';
}

//code taken from online source
int startsWith(const char *str, const char *pre)
{
    size_t lenpre = strlen(pre);
    size_t lenstr = strlen(str);
    return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}

//From B&O
//Check fork system call
pid_t Fork(void)
{
    pid_t pid;

    if ((pid = fork()) < 0)
        unix_error("Fork error");
    return pid;
}

//body of method taken from Project 0 instructions
//Check write system call
void Write(char* str, int numBytes) 
{
    const int STDOUT = 1;
    ssize_t bytes = write(STDOUT, str, numBytes);
    if(bytes != numBytes)
        unix_error("Write error");
    return;
}

//Check nanosleep system call
int Nanosleep(const struct timespec *req, struct timespec *rem)
{
    int retval = nanosleep(req, rem);
    if(errno == EFAULT || errno == EINVAL)
        unix_error("nanosleep error");
    return retval;
}

//Check sigemptyset system call
void Sigemptyset(sigset_t *mask) 
{
    if(sigemptyset(mask) == -1)
        unix_error("sigemptyset error");
}

//Check sigaddset system call
void Sigaddset(sigset_t *mask, int sig)
{
    if(sigaddset(mask, sig) == -1)
        unix_error("sigaddset error");
}

//Check sigprocmask system call
void Sigprocmask(int sig, sigset_t *mask, sigset_t *prev)
{
    if(sigprocmask(sig, mask, prev) == -1)
        unix_error("sigprocmask error");
}

//Check setpgid system call
void Setpgid(pid_t pid, pid_t pgid)
{
    if(setpgid(pid, pgid) == -1)
        unix_error("setpgid error");
}

//Check sigsuspend system call
void Sigsuspend(sigset_t *prev)
{
    sigsuspend(prev);
    if(errno != EINTR)
        unix_error("sigsuspend error");
}

//Check kill system call
void Kill(pid_t pid, int sig)
{
    if(kill(-pid, sig) == -1)
        unix_error("kill error");
}

//Check sprintf system call
ssize_t Sprintf(char *buffer, char *str, int jid, pid_t pid)
{
   ssize_t bytes = sprintf(buffer, str, jid, pid);
   if(bytes < 0)
       unix_error("sprintf error");
   return bytes;
}

//Check waitpid system call
pid_t Waitpid(pid_t pid, int *status, int options)
{
   pid_t retval = waitpid(pid, status, options);
   if(errno != ECHILD && retval < 0)
       unix_error("waitpid error");
   return retval;
}

//Checking execve system call in msh code
