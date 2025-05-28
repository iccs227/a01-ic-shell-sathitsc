/* ICCS227: Project 1: icsh
* Name: Chinanard Sathitseth
 * StudentID: 6481366
 */
//Milestone 4
#ifndef ICSH_SIGNAL_HANDLER_H
#define ICSH_SIGNAL_HANDLER_H

extern pid_t fg_pid;

void signal_handler(int is_signal);
void implement_signal_handlers();

#endif
