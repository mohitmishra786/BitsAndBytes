#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_QUEUES 3
#define MAX_PROCESSES 100

typedef struct {
    int pid;
    int burst_time;
    int remaining_time;
    int queue_level;
    int quantum_used;
} MLFQProcess;

typedef struct {
    MLFQProcess* processes[MAX_PROCESSES];
    int count;
    int quantum;
} Queue;

typedef struct {
    Queue queues[MAX_QUEUES];
    int current_queue;
} MLFQ;

// Initialize MLFQ system
MLFQ* init_mlfq() {
    MLFQ* mlfq = (MLFQ*)malloc(sizeof(MLFQ));
    for (int i = 0; i < MAX_QUEUES; i++) {
        mlfq->queues[i].count = 0;
        // Quantum increases exponentially with queue level
        mlfq->queues[i].quantum = 1 << i;
    }
    mlfq->current_queue = 0;
    return mlfq;
}

// Add process to MLFQ
void add_process(MLFQ* mlfq, int pid, int burst_time) {
    MLFQProcess* process = (MLFQProcess*)malloc(sizeof(MLFQProcess));
    process->pid = pid;
    process->burst_time = burst_time;
    process->remaining_time = burst_time;
    process->queue_level = 0;
    process->quantum_used = 0;
    
    Queue* top_queue = &mlfq->queues[0];
    top_queue->processes[top_queue->count++] = process;
}

// Execute one time slice in MLFQ
void execute_time_slice(MLFQ* mlfq) {
    for (int i = 0; i < MAX_QUEUES; i++) {
        Queue* current_queue = &mlfq->queues[i];
        
        if (current_queue->count > 0) {
            MLFQProcess* process = current_queue->processes[0];
            process->remaining_time--;
            process->quantum_used++;
            
            printf("Executing PID %d in Queue %d (Remaining: %d)\n",
                   process->pid, i, process->remaining_time);
            
            // Process completed
            if (process->remaining_time == 0) {
                printf("Process %d completed\n", process->pid);
                // Remove process from queue
                for (int j = 0; j < current_queue->count - 1; j++) {
                    current_queue->processes[j] = current_queue->processes[j + 1];
                }
                current_queue->count--;
                free(process);
            }
            // Process used its quantum
            else if (process->quantum_used >= current_queue->quantum) {
                // Move to next queue if not already at lowest
                if (i < MAX_QUEUES - 1) {
                    process->queue_level++;
                    process->quantum_used = 0;
                    Queue* next_queue = &mlfq->queues[i + 1];
                    next_queue->processes[next_queue->count++] = process;
                    
                    // Remove from current queue
                    for (int j = 0; j < current_queue->count - 1; j++) {
                        current_queue->processes[j] = current_queue->processes[j + 1];
                    }
                    current_queue->count--;
                }
            }
            return;
        }
    }
}

// Run MLFQ simulation
void run_mlfq(MLFQ* mlfq) {
    int total_processes;
    bool processes_remaining;
    
    do {
        processes_remaining = false;
        total_processes = 0;
        
        for (int i = 0; i < MAX_QUEUES; i++) {
            total_processes += mlfq->queues[i].count;
            if (mlfq->queues[i].count > 0) {
                processes_remaining = true;
            }
        }
        
        if (processes_remaining) {
            execute_time_slice(mlfq);
        }
        
    } while (processes_remaining);
}

int main() {
    MLFQ* mlfq = init_mlfq();
    
    // Add test processes
    add_process(mlfq, 1, 8);   // Long process
    add_process(mlfq, 2, 4);   // Medium process
    add_process(mlfq, 3, 2);   // Short process
    
    printf("Starting MLFQ Simulation\n");
    printf("Queue 0 Quantum: 1\n");
    printf("Queue 1 Quantum: 2\n");
    printf("Queue 2 Quantum: 4\n");
    
    run_mlfq(mlfq);
    
    free(mlfq);
    return 0;
}