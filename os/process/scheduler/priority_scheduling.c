#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pid;
    int priority;
    int age;
    int burst_time;
    int remaining_time;
} PriorityProcess;

void age_processes(PriorityProcess processes[], int n) {
    for (int i = 0; i < n; i++) {
        if (processes[i].remaining_time > 0) {
            processes[i].age++;
            if (processes[i].age >= 5) {  // Age threshold
                processes[i].priority++;
                processes[i].age = 0;
                printf("Process %d priority boosted to %d\n", 
                       processes[i].pid, 
                       processes[i].priority);
            }
        }
    }
}

int get_highest_priority_process(PriorityProcess processes[], int n) {
    int highest_priority = -1;
    int selected_process = -1;
    
    for (int i = 0; i < n; i++) {
        if (processes[i].remaining_time > 0) {
            if (selected_process == -1 || 
                processes[i].priority > highest_priority) {
                highest_priority = processes[i].priority;
                selected_process = i;
            }
        }
    }
    
    return selected_process;
}

void priority_schedule_with_aging(PriorityProcess processes[], int n) {
    int current_time = 0;
    int completed = 0;
    
    while (completed < n) {
        age_processes(processes, n);
        int current_process = get_highest_priority_process(processes, n);
        
        if (current_process == -1) {
            current_time++;
            continue;
        }
        
        // Execute process for one time unit
        processes[current_process].remaining_time--;
        current_time++;
        
        if (processes[current_process].remaining_time == 0) {
            completed++;
            printf("Process %d completed at time %d\n", 
                   processes[current_process].pid, 
                   current_time);
        }
    }
}