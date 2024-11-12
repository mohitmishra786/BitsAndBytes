#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

#define QUANTUM_MS 100

typedef struct {
    int pid;
    int remaining_time;
} Task;

Task* current_task = NULL;
int is_running = 1;

void timer_handler(int signum) {
    if (current_task != NULL) {
        printf("Preempting task %d (remaining time: %d ms)\n", 
               current_task->pid, 
               current_task->remaining_time);
        current_task->remaining_time -= QUANTUM_MS;
        
        if (current_task->remaining_time <= 0) {
            printf("Task %d completed\n", current_task->pid);
            free(current_task);
            current_task = NULL;
        }
    }
}

void setup_timer() {
    struct sigaction sa;
    struct itimerval timer;

    // Install timer_handler as the signal handler for SIGVTALRM
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &timer_handler;
    sigaction(SIGVTALRM, &sa, NULL);

    // Configure the timer to expire after QUANTUM_MS msec
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = QUANTUM_MS * 1000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = QUANTUM_MS * 1000;

    // Start the timer
    setitimer(SIGVTALRM, &timer, NULL);
}

Task* create_task(int pid, int execution_time) {
    Task* task = (Task*)malloc(sizeof(Task));
    task->pid = pid;
    task->remaining_time = execution_time;
    return task;
}

int main() {
    setup_timer();
    
    // Create and run a sample task
    current_task = create_task(1, 500);
    printf("Starting task %d (execution time: %d ms)\n", 
           current_task->pid, 
           current_task->remaining_time);
    
    // Simple execution loop
    while (is_running && current_task != NULL) {
        // Simulate CPU work
        usleep(10000);
    }
    
    printf("Scheduler terminated\n");
    return 0;
}
