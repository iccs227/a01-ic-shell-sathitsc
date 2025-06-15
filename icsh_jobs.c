/* ICCS227: Project 1: icsh
 * Name: Chinanard Sathitseth
 * StudentID: 6481366
 */

#include "icsh_jobs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

#define MAX_JOBS 64

static Job jobs[MAX_JOBS];
static int num_jobs = 0;
static int next_job_id = 1;


void add_job(pid_t pid, const char *cmd, JobStatus status, bool is_background) {
    if (num_jobs >= MAX_JOBS) return;
    jobs[num_jobs].job_id = next_job_id++;
    jobs[num_jobs].pid = pid;
    strncpy(jobs[num_jobs].command_line, cmd, 255);
    jobs[num_jobs].command_line[255] = '\0';
    jobs[num_jobs].status = status;
    jobs[num_jobs].is_background = is_background;
    num_jobs++;
}

/**
 * Remove a job from the jobs list by PID.
 */
void remove_job(pid_t pid) {
    for (int i = 0; i < num_jobs; ++i) {
        if (jobs[i].pid == pid) {
            for (int j = i; j < num_jobs - 1; ++j) {
                jobs[j] = jobs[j + 1];
            }
            num_jobs--;
            return;
        }
    }
}

/**
 * Print the list of active jobs.
 */
void list_jobs() {
    for (int i = 0; i < num_jobs; ++i) {
        const char *status_str = (jobs[i].status == Running) ? "Running" :
                                 (jobs[i].status == Stopped) ? "Stopped" : "Done";
        printf("[%d] %s\t%s%s\n", jobs[i].job_id, status_str, jobs[i].command_line,
               jobs[i].is_background ? " &" : "");
    }
}

/**
 * Find a job by job ID.
 */
Job *find_job_by_id(int job_id) {
    for (int i = 0; i < num_jobs; ++i) {
        if (jobs[i].job_id == job_id) return &jobs[i];
    }
    return NULL;
}

/**
 * Find a job by PID.
 */
Job *find_job_by_pid(pid_t pid) {
    for (int i = 0; i < num_jobs; ++i) {
        if (jobs[i].pid == pid) return &jobs[i];
    }
    return NULL;
}

/**
 * Update the status of a job by PID.
 */
void update_job_status(pid_t pid, JobStatus status) {
    for (int i = 0; i < num_jobs; ++i) {
        if (jobs[i].pid == pid) {
            jobs[i].status = status;
            return;
        }
    }
}

/**
 * Get the next job ID (not used directly for job creation).
 */
int get_next_job_id() {
    return next_job_id;
}

/**
 * Get the last assigned job ID.
 */
int get_last_job_id() {
    return next_job_id - 1;
}

/**
 * Bring a stopped or background job to the foreground and wait for it to finish or stop.
 */
void fg_command(int job_id) {
    Job *job = find_job_by_id(job_id);
    if (!job) {
        printf("fg: job %% %d not found\n", job_id);
        return;
    }

    pid_t pid = job->pid;

    // Continue if stopped
    if (job->status == Stopped) {
        if (kill(-pid, SIGCONT) < 0) {
            perror("kill(SIGCONT)");
        }
        job->status = Running;
    }

    // Set the foreground pid for signal handling
    extern pid_t fg_pid;
    fg_pid = pid;

    if (tcsetpgrp(STDIN_FILENO, pid) == -1) {
        perror("tcsetpgrp");
    }

    int status;
    if (waitpid(pid, &status, WUNTRACED) == -1) {
        perror("waitpid");
    }

    if (tcsetpgrp(STDIN_FILENO, getpid()) == -1) {
        perror("tcsetpgrp");
    }

    if (WIFSTOPPED(status)) {
        job->status = Stopped;
        printf("\n[%d]+  Stopped\t\t%s\n", job->job_id, job->command_line);
    } else {
        job->status = Done;
        printf("\n[%d]+  Done\t\t%s\n", job->job_id, job->command_line);
        remove_job(pid);
    }

    fg_pid = -1;  // Reset foreground pid
}

/**
 * Resume a stopped job in the background.
 */
void bg_command(int job_id) {
    Job *job = find_job_by_id(job_id);
    if (!job) {
        printf("bg: job %% %d not found\n", job_id);
        return;
    }

    if (job->status != Stopped) {
        printf("bg: job %% %d not stopped\n", job_id);
        return;
    }

    if (kill(-job->pid, SIGCONT) < 0) {
        perror("kill(SIGCONT)");
    }

    job->status = Running;
    printf("[%d]+ %s &\n", job->job_id, job->command_line);
}
