/* ICCS227: Project 1: icsh
* Name: Chinanard Sathitseth
 * StudentID: 6481366
 */

#ifndef ICSH_JOBS_H
#define ICSH_JOBS_H

#include <stdbool.h>
#include <sys/types.h>  // for pid_t

#define MAX_COMMAND_LENGTH 256

typedef enum {
    Running,
    Stopped,
    Done
} JobStatus;

typedef struct {
    int job_id;
    pid_t pid;
    char command_line[256];
    JobStatus status;
    bool is_background;
} Job;

void add_job(pid_t pid, const char *cmd, JobStatus status, bool is_background);
void remove_job(pid_t pid);
void list_jobs(void);
Job *find_job_by_id(int job_id);
Job *find_job_by_pid(pid_t pid);
void update_job_status(pid_t pid, JobStatus status);
int get_next_job_id(void);

#endif
