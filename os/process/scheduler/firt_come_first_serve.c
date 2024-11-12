#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
} Process;

void fcfs_schedule(Process processes[], int n) {
    int current_time = 0;
    
    for (int i = 0; i < n; i++) {
        // If current time is less than arrival time, CPU will be idle
        if (current_time < processes[i].arrival_time)
            current_time = processes[i].arrival_time;
            
        processes[i].completion_time = current_time + processes[i].burst_time;
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
        
        current_time = processes[i].completion_time;
    }
}

void print_metrics(Process processes[], int n) {
    float avg_turnaround = 0, avg_waiting = 0;
    
    printf("\nProcess Metrics:\n");
    printf("PID\tArrival\tBurst\tComplete\tTurnaround\tWaiting\n");
    
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t\t%d\t\t%d\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].completion_time,
               processes[i].turnaround_time,
               processes[i].waiting_time);
               
        avg_turnaround += processes[i].turnaround_time;
        avg_waiting += processes[i].waiting_time;
    }
    
    printf("\nAverage Turnaround Time: %.2f\n", avg_turnaround/n);
    printf("Average Waiting Time: %.2f\n", avg_waiting/n);
}

int main() {
    Process processes[] = {
        {1, 0, 7, 0, 0, 0},
        {2, 2, 4, 0, 0, 0},
        {3, 4, 1, 0, 0, 0}
    };
    int n = sizeof(processes)/sizeof(processes[0]);
    
    fcfs_schedule(processes, n);
    print_metrics(processes, n);
    
    return 0;
}