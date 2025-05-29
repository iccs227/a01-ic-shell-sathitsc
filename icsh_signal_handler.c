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

pid_t fg_pid = -1;                      //no foreground process is running

void signal_handler(const int is_signal) {
    if (fg_pid != -1) {
        kill(-fg_pid, is_signal);
    } else {                            //Check which signal is being received.
        if (is_signal == SIGINT) {                  //Ctrl-C
            write(STDOUT_FILENO, "\nicsh $ ", 8);
        } else if (is_signal == SIGTSTP) {          //Ctrl-Z
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
