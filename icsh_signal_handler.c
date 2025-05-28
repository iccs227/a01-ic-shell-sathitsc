/* ICCS227: Project 1: icsh
* Name: Chinanard Sathitseth
 * StudentID: 6481366
 */

// https://stackoverflow.com/questions/2485028/signal-handling-in-c
// https://www.devzery.com/post/your-ultimate-guide-to-c-handlers
//https://www.ibm.com/docs/en/zos/3.1.0?topic=functions-wait-wait-child-process-end (very good one)
//https://people.cs.rutgers.edu/~pxk/416/notes/c-tutorials/wait.html
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

pid_t fg_pid = -1; //no foreground process is running
void signal_handler(const int is_signal) {
    if (fg_pid != -1) {
        kill(-fg_pid, is_signal);
    } else { //Check which signal is being received.
        if (is_signal == SIGINT) { //Ctrl-C
            write(STDOUT_FILENO, "\nicsh $ ", 8);
        } else if (is_signal == SIGTSTP) { //Ctrl-Z
            write(STDOUT_FILENO, "\nicsh $ ", 8);
        }
    }
}
//https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-sigemptyset-initialize-signal-mask-exclude-all-signals
void implement_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask); // no other signals are blocked when the handler is running.
    sa.sa_flags = SA_RESTART;
    /*If a signal interrupts a system call (like read(), wait(), or write()),
     *just automatically try again instead of making it fail.
     *https://pubs.opengroup.org/onlinepubs/9799919799/functions/sigaction.html
     */
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
