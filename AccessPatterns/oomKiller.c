#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_PROCESSES 100
#define MAX_NAME_LENGTH 50

typedef struct {
    pid_t pid;
    char name[MAX_NAME_LENGTH];
    unsigned long rss;
    unsigned long virtual_size;
    long cpu_time;
    time_t start_time;
    int nice;
    int oom_score_adj;
    int is_privileged;
} Process;

typedef struct {
    Process processes[MAX_PROCESSES];
    int count;
} ProcessList;

void initialize_process_list(ProcessList *list);
void add_process(ProcessList *list, pid_t pid, const char *name, unsigned long rss, 
                 unsigned long virtual_size, long cpu_time, time_t start_time, 
                 int nice, int oom_score_adj, int is_privileged);
float calculate_oom_score(Process *process, time_t current_time);
Process* select_oom_victim(ProcessList *list);
void terminate_process(Process *victim);

int main() {
    ProcessList process_list;
    initialize_process_list(&process_list);

    add_process(&process_list, 1001, "web_server", 1024000, 2048000, 5000, time(NULL) - 3600, 0, 0, 0);
    add_process(&process_list, 1002, "database", 2048000, 4096000, 10000, time(NULL) - 7200, -5, -100, 1);
    add_process(&process_list, 1003, "user_app", 512000, 1024000, 2000, time(NULL) - 1800, 0, 0, 0);

    Process *victim = select_oom_victim(&process_list);

    if (victim) {
        printf("OOM Killer selected process %d (%s) for termination\n", victim->pid, victim->name);
        terminate_process(victim);
    } else {
        printf("No suitable process found for termination\n");
    }

    return 0;
}

void initialize_process_list(ProcessList *list) {
    list->count = 0;
}

void add_process(ProcessList *list, pid_t pid, const char *name, unsigned long rss, 
                 unsigned long virtual_size, long cpu_time, time_t start_time, 
                 int nice, int oom_score_adj, int is_privileged) {
    if (list->count < MAX_PROCESSES) {
        Process *p = &list->processes[list->count++];
        p->pid = pid;
        strncpy(p->name, name, MAX_NAME_LENGTH - 1);
        p->name[MAX_NAME_LENGTH - 1] = '\0';
        p->rss = rss;
        p->virtual_size = virtual_size;
        p->cpu_time = cpu_time;
        p->start_time = start_time;
        p->nice = nice;
        p->oom_score_adj = oom_score_adj;
        p->is_privileged = is_privileged;
    }
}

float calculate_oom_score(Process *process, time_t current_time) {
    float score = 0;

    // Memory score (logarithmic scale)
    score += 10 * log2f((float)process->rss / 1024);  // RSS in MB

    // CPU time score
    score += log2f((float)process->cpu_time / 1000);  // CPU time in seconds

    // Process lifetime score
    long lifetime = current_time - process->start_time;
    score -= log2f((float)lifetime / 3600);  // Lifetime in hours

    // Nice value adjustment
    score += process->nice;

    // OOM score adjustment
    score += process->oom_score_adj;

    // Privileged process adjustment
    if (process->is_privileged) {
        score -= 100;
    }

    return score;
}

Process* select_oom_victim(ProcessList *list) {
    Process *victim = NULL;
    float max_score = -1000000;  // Initialize to a very low value
    time_t current_time = time(NULL);

    for (int i = 0; i < list->count; i++) {
        Process *p = &list->processes[i];
        float score = calculate_oom_score(p, current_time);

        if (score > max_score) {
            max_score = score;
            victim = p;
        }
    }

    return victim;
}

void terminate_process(Process *victim) {
    printf("Simulating termination of process %d (%s)\n", victim->pid, victim->name);
    // In a real implementation, this would send a SIGKILL to the process
    // kill(victim->pid, SIGKILL);
}
