/* ICCS227: Project 1: icsh
 * Name: Chinanard Sathitseth
 * StudentID: 6481366
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "icsh_builtins.h"
#include "icsh_jobs.h"


extern pid_t fg_pid;

bool is_redirection(char **args) {
    // check if an entire command line contains a valid redirection, not only operator (different from external command ones)
    for (int i = 0; args[i] != NULL; i++) {
        if ((strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0 || strcmp(args[i], "<") == 0)) {
            if (args[i + 1] != NULL && strcmp(args[i + 1], ">") != 0 &&
                strcmp(args[i + 1], ">>") != 0 && strcmp(args[i + 1], "<") != 0) {
                return true;
            }
        }
    }
    return false;
}

bool handle_builtin(char **args, int *exit_code) {
    if (args[0] == NULL) return true;

    // Milestone 1: echo
    if (strcmp(args[0], "echo") == 0 && !is_redirection(args)) {
        for (int i = 1; args[i] != NULL; i++) {
            printf("%s", args[i]);
            if (args[i + 1] != NULL) printf(" ");
        }
        printf("\n");
        *exit_code = 0;
        return true;
    }

    // Milestone 6: jobs
    if (strcmp(args[0], "jobs") == 0) {
        list_jobs();
        *exit_code = 0;
        return true;
    }

    // Milestone 6: fg %n
    if (strcmp(args[0], "fg") == 0 && args[1] && args[1][0] == '%') {
        int job_id = atoi(&args[1][1]);  // extract number after %
        Job *job = find_job_by_id(job_id);
        if (job) {
            fg_pid = job->pid;
            job->status = Running;
            kill(-job->pid, SIGCONT);

            int status;
            waitpid(job->pid, &status, WUNTRACED);
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                remove_job(job->pid);
            } else if (WIFSTOPPED(status)) {
                update_job_status(job->pid, Stopped);
            }
            fg_pid = -1;
        } else {
            printf("fg: job %s not found\n", args[1]);
        }
        *exit_code = 0;
        return true;
    }

    // Milestone 6: bg %n
    if (strcmp(args[0], "bg") == 0 && args[1] && args[1][0] == '%') {
        int job_id = atoi(&args[1][1]);
        Job *job = find_job_by_id(job_id);
        if (job && job->status == Stopped) {
            kill(-job->pid, SIGCONT);
            job->status = Running;
            printf("[%d]+ %s &\n", job->job_id, job->command_line);
        } else {
            printf("bg: job %s not found or not stopped\n", args[1]);
        }
        *exit_code = 0;
        return true;
    }

    // Exit command
    if (strcmp(args[0], "exit") == 0) {
        int code = 0;
        if (args[1] != NULL) {
            code = atoi(args[1]) & 0xFF;
        }
        printf("⊱  ۫ ׅ ✧ ─── ⋆⋅☆⋅⋆ ───── ⋆⋅☆⋅⋆ ── bye ─── ⋆⋅☆⋅⋆ ───── ⋆⋅☆⋅⋆ ── ⊱  ۫ ׅ ✧ \n");
        *exit_code = code;
        return true;
    }

    return false;
}
