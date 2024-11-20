#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

// Structure to hold timing information
typedef struct {
    struct timeval start;
    struct timeval end;
} TimingInfo;

// Function to start timing
void start_timing(TimingInfo* timing) {
    gettimeofday(&timing->start, NULL);
}

// Function to end timing and return microseconds elapsed
long end_timing(TimingInfo* timing) {
    gettimeofday(&timing->end, NULL);
    return (timing->end.tv_sec - timing->start.tv_sec) * 1000000 +
           (timing->end.tv_usec - timing->start.tv_usec);
}

int main() {
    const int NUM_ALLOCATIONS = 10000;
    const int ALLOCATION_SIZE = 1024; // 1KB
    TimingInfo timing;
    void* allocations[NUM_ALLOCATIONS];
    
    // Measure heap allocations
    start_timing(&timing);
    for(int i = 0; i < NUM_ALLOCATIONS; i++) {
        allocations[i] = malloc(ALLOCATION_SIZE);
        if(!allocations[i]) {
            printf("Allocation failed!\n");
            exit(1);
        }
    }
    long heap_time = end_timing(&timing);
    
    // Clean up
    for(int i = 0; i < NUM_ALLOCATIONS; i++) {
        free(allocations[i]);
    }
    
    // Measure stack allocations
    start_timing(&timing);
    for(int i = 0; i < NUM_ALLOCATIONS; i++) {
        char stack_array[ALLOCATION_SIZE];
        stack_array[0] = 1; // Prevent optimization
    }
    long stack_time = end_timing(&timing);
    
    printf("Time for %d heap allocations: %ld microseconds\n", 
           NUM_ALLOCATIONS, heap_time);
    printf("Time for %d stack allocations: %ld microseconds\n", 
           NUM_ALLOCATIONS, stack_time);
    printf("Ratio (heap/stack): %.2fx\n", 
           (double)heap_time / stack_time);
    
    return 0;
}