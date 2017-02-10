#ifndef _UTIL_H_
#define _UTIL_H_
/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

int parseline(const char *cmdline, char **argv); 
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);
void substr(char dest[], char src[], int offset, int len);
int startsWith(const char *str, const char *pre);
pid_t Fork(void);
void Write(char* str, int numBytes);
int Nanosleep(const struct timespec *req, struct timespec *rem);
void Sigemptyset(sigset_t *mask);
void Sigaddset(sigset_t *mask, int sig);
void Sigprocmask(int sig, sigset_t *mask, sigset_t *prev);
void Setpgid(pid_t pid, pid_t pgid);
void Sigsuspend(sigset_t *prev);
void Kill(pid_t pid, int sig);
ssize_t Sprintf(char *buffer, char *str, int jid, pid_t pid);
pid_t Waitpid(pid_t pid, int *status, int options);
//exec is checked in msh code

#endif
