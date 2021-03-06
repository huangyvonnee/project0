/* 
 * msh - A mini shell program with job control
 * 
 * <Yvonne Huang: yvonnee>
 * <Mohammad Asif: masif96>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "util.h"
#include "jobs.h"

/* Global variables */
int verbose = 0;            /* if true, print additional output */

extern char **environ;      /* defined in libc */
static char prompt[] = "msh> ";    /* command line prompt (DO NOT CHANGE) */
static struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
void usage(void);
void sigquit_handler(int sig);



/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
        break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
        break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
        break;
    default:
            usage();
    }
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1) {

    /* Read command line */
    if (emit_prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }
    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
        app_error("fgets error");
    if (feof(stdin)) { /* End of file (ctrl-d) */
        fflush(stdout);
        exit(0);
    }

    /* Evaluate the command line */
    eval(cmdline);
    fflush(stdout);
    fflush(stdout);
    } 

    exit(0); /* control never reaches here */
}
  
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.
 * 
 * Structure of code taken from B&O
*/
void eval(char *cmdline) {
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int state;
    pid_t pid;

    strcpy(buf, cmdline);
    if(parseline(buf, argv))
        state = BG;
    else
        state = FG;

    if(argv[0] == NULL)
        return;

    if(!builtin_cmd(argv)) {

        sigset_t mask;
        Sigemptyset(&mask);
        Sigaddset(&mask, SIGCHLD);
        Sigprocmask(SIG_BLOCK, &mask, NULL);

        if((pid = Fork()) == 0) {
            Sigprocmask(SIG_UNBLOCK, &mask, NULL);
            Setpgid(0,0);

            if(execve(argv[0], argv, environ) < 0) {
                //unix_error(argv[0]);
                printf("%s: Command not found\n", argv[0]);
                exit(0);
            }

        } else {
            addjob(jobs, pid, state, cmdline);
            Sigprocmask(SIG_UNBLOCK, &mask, NULL);

            if(state == FG)
                waitfg(fgpid(jobs));
            else
                showjobstatus(jobs, pid);
        }
    }
    return;
}

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 * Return 1 if a builtin command was executed; return 0
 * if the argument passed in is *not* a builtin command.
 */
int builtin_cmd(char **argv) 
{
    //Yvonne driving now
    if(!strcmp(argv[0], "quit"))
        exit(0);
    if(!strcmp(argv[0], "&"))
        return 1;
    if(!strcmp(argv[0], "jobs")){
       listjobs(jobs);
       return 1;
    }
    if(!strcmp(argv[0], "bg")){
       do_bgfg(argv);
       return 1;
    }
    if(!strcmp(argv[0], "fg")){
       do_bgfg(argv);
       return 1;
    }

    return 0;     /* not a builtin command */
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) 
{
    //Yvonne driving now
    //Check if user only wrote "bg" or "fg"
    if(argv[1] == NULL){
        if(!strcmp(argv[0], "bg"))
            printf("bg command requires PID or %%jobid argument\n");
        if(!strcmp(argv[0], "fg"))
            printf("fg command requires PID or %%jobid argument\n");
        return;
    }

    //Mohammad driving now
    //Check if argument entered is a PID or a JID
    //If arg starts with a "%", it's a JID, else PID
    int len = strlen(argv[1]);
    if(startsWith(argv[1], "%")) {
        char num[len-1];
        substr(num, argv[1], 1, len-1);
        int jid = atoi(num);
        //Checks if user entered a valid JID
        if(getjobjid(jobs, jid) == NULL){
            printf("%%%d: No such job\n", jid);
            return;
        }
        pid_t pid = jid2pid(jobs, jid);
        Kill(-pid, SIGCONT);
        //Checks if user wants program to run in foreground or background
        if(!strcmp(argv[0], "bg")) {
            updatestate(jobs, pid, BG);
            showjobstatus(jobs, pid);
        }
        if(!strcmp(argv[0], "fg")) {
            updatestate(jobs, pid, FG);
            waitfg(fgpid(jobs));
        }
    } else {
        //Yvonne driving now
        //Check if user entered a number or not as the PID argument
        for(int i =0; i < len; i++){
            if(!isdigit(argv[1][i])){
                if(!strcmp(argv[0], "bg"))
                    printf("bg: argument must be a PID or %%jobid\n");
                if(!strcmp(argv[0], "fg"))
                    printf("fg: argument must be a PID or %%jobid\n");

                return;
            }
        }

        //Check if user entered valid PID
        pid_t pid = atoi(argv[1]);
        if(getjobpid(jobs, pid) == NULL){
            printf("(%d): No such process\n", pid);
            return;
        }

        Kill(-pid, SIGCONT);
        //Checks if user wants program to run in foreground or background
        if(!strcmp(argv[0], "bg"))
            updatestate(jobs, pid, BG);
        if(!strcmp(argv[0], "fg")) {
            updatestate(jobs, pid, FG);
            waitfg(fgpid(jobs));
        }
    }

    return;
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 * 
 * some code taken from B&O
 */
void waitfg(pid_t pid)
{
    //Mohammad and Yvonne driving now
    sigset_t mask, prev;
    Sigemptyset(&mask);
    Sigemptyset(&prev);
    Sigaddset(&mask, SIGTSTP);
    Sigaddset(&mask, SIGINT);
    
    Sigprocmask(SIG_BLOCK, &mask, NULL);
    while(pid == fgpid(jobs)){    
        Sigsuspend(&prev);
    }
    Sigprocmask(SIG_UNBLOCK, &mask, NULL);
    return;
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig) 
{
    //Mohammad driving now
    int status;
    pid_t pid;
    ssize_t bytes;
    char buffer[MAXLINE];

    while((pid = Waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0){
       if(WIFSIGNALED(status)){
            int jid = pid2jid(jobs, fgpid(jobs));
            bytes = Sprintf(buffer, "Job [%d] (%d) terminated by signal 2\n", jid, fgpid(jobs));
            Write(buffer, bytes);
            deletejob(jobs, pid);
        }
        else if(WIFSTOPPED(status)){
            int jid = pid2jid(jobs, pid);
            bytes = Sprintf(buffer, "Job [%d] (%d) stopped by signal 20\n", jid, pid);
            Write(buffer, bytes);
            updatestate(jobs, pid, ST);
        }
        else if(WIFEXITED(status)){
            deletejob(jobs,pid);
        }
    }
    return;
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{
    //Yvonne driving now
    pid_t pid = fgpid(jobs);
    if(pid != 0)
        Kill(-pid, SIGINT);
    return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
    //Yvonne driving now
    pid_t pid = fgpid(jobs);
    if(pid != 0)
        Kill(-pid, SIGTSTP);
    return;
}

/*********************
 * End signal handlers
 *********************/



/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
    ssize_t bytes;
    const int STDOUT = 1;
    bytes = write(STDOUT, "Terminating after receipt of SIGQUIT signal\n", 45);
    if(bytes != 45)
       exit(-999);
    exit(1);
}
