#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    CREATED,
    RUNNING,
    WAITING,
    READY,
    TERMINATED
} ProcessState;

typedef struct {
    int pid;
    ProcessState state;
    int cpu_burst_time;
    int io_burst_time;
} Process;

// Function to create a new process
Process* create_process(int pid, int cpu_time, int io_time) {
    Process* p = (Process*)malloc(sizeof(Process));
    p->pid = pid;
    p->state = CREATED;
    p->cpu_burst_time = cpu_time;
    p->io_burst_time = io_time;
    return p;
}

// Function to transition process state
void transition_state(Process* p, ProcessState new_state) {
    printf("Process %d: %d -> %d\n", p->pid, p->state, new_state);
    p->state = new_state;
}

// Function to simulate process execution
void simulate_process(Process* p) {
    transition_state(p, READY);
    transition_state(p, RUNNING);
    
    if (p->io_burst_time > 0) {
        transition_state(p, WAITING);
        transition_state(p, READY);
        transition_state(p, RUNNING);
    }
    
    transition_state(p, TERMINATED);
}

int main() {
    Process* p1 = create_process(1, 100, 50);
    Process* p2 = create_process(2, 75, 0);
    
    printf("Simulating process 1 (CPU-I/O bound):\n");
    simulate_process(p1);
    
    printf("\nSimulating process 2 (CPU bound):\n");
    simulate_process(p2);
    
    free(p1);
    free(p2);
    return 0;
}
