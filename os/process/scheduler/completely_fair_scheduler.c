#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#define MIN_NICE -20
#define MAX_NICE 19

typedef struct {
    int pid;
    int nice_value;
    unsigned long vruntime;    // Virtual runtime
    unsigned long weight;      // Process weight based on nice value
} CFSProcess;

// Calculate process weight based on nice value
unsigned long calculate_weight(int nice) {
    // Simplified weight calculation
    return (MAX_NICE - nice + 1) * 100;
}

// Update virtual runtime based on actual runtime and weight
void update_vruntime(CFSProcess* process, unsigned long runtime) {
    process->vruntime += (runtime * 1024) / process->weight;
}

// Find process with minimum vruntime
int find_min_vruntime_process(CFSProcess* processes, int count) {
    int min_idx = 0;
    unsigned long min_vruntime = processes[0].vruntime;
    
    for (int i = 1; i < count; i++) {
        if (processes[i].vruntime < min_vruntime) {
            min_vruntime = processes[i].vruntime;
            min_idx = i;
        }
    }
    
    return min_idx;
}

void simulate_cfs(CFSProcess* processes, int count, int simulation_time) {
    printf("Starting CFS Simulation\n");
    
    // Initialize process weights
    for (int i = 0; i < count; i++) {
        processes[i].weight = calculate_weight(processes[i].nice_value);
        processes[i].vruntime = 0;
    }
    
    // Simulation loop
    for (int time = 0; time < simulation_time; time++) {
        int next_process = find_min_vruntime_process(processes, count);
        
        printf("Time %d: Running PID %d (Nice: %d, VRuntime: %lu)\n",
               time,
               processes[next_process].pid,
               processes[next_process].nice_value,
               processes[next_process].vruntime);
               
        // Simulate execution and update vruntime
        update_vruntime(&processes[next_process], 1);
    }
}

int main() {
    CFSProcess processes[] = {
        {1, 0, 0, 0},     // Default nice value
        {2, -10, 0, 0},   // High priority
        {3, 10, 0, 0}     // Low priority
    };
    int process_count = sizeof(processes) / sizeof(processes[0]);
    
    simulate_cfs(processes, process_count, 15);
    
    return 0;
}