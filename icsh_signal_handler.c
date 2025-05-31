/* ICCS227: Project 1: icsh
* Name: Chinanard Sathitseth
 * StudentID: 6481366
 */

/**
*Note (•˕ •マ.ᐟ
*Ctrl-C → send SIGINT (interrupt signal)
*Ctrl-Z → send SIGTSTP (stop signal)
 */
//Milestone 4
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "icsh_signal_handler.h"
#include "icsh_jobs.h"
#include <errno.h>
#include <sys/wait.h>

pid_t fg_pid = -1;                      //no foreground process is running

void signal_handler(const int is_signal) {
    if (fg_pid != -1) {
        kill(-fg_pid, is_signal);  // send signal to entire fg process group
    } else {
        if (is_signal == SIGINT || is_signal == SIGTSTP) {
            write(STDOUT_FILENO, "\nicsh $ ", 8);
        }
    }
}
void implement_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);       // no other signals are blocked when the handler is running.
    sa.sa_flags = SA_RESTART;

    //Ctrl-C (SIGINT)
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction SIGINT");
        exit(EXIT_FAILURE);
    }
    //Ctrl-Z (SIGTSTP)
    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        perror("sigaction SIGTSTP");
        exit(EXIT_FAILURE);
    }
}

void sigchld_handler(int sig) {
    int saved_errno = errno;
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        Job *job = find_job_by_pid(pid);
        if (!job) continue;

        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            job->status = Done;
            printf("\n[%d] Done\t\t%s\nicsh $ ", job->job_id, job->command_line);
            fflush(stdout);
            remove_job(pid);
        } else if (WIFSTOPPED(status)) {
            job->status = Stopped;
            printf("\n[%d]+  Stopped\t%s\nicsh $ ", job->job_id, job->command_line);
            fflush(stdout);
        } else if (WIFCONTINUED(status)) {
            job->status = Running;
        }
    }

    errno = saved_errno;
}
