#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

#define NUM_ITERATIONS 1000

double measure_context_switch() {
    int pipefd[2];
    struct timeval start, end;
    char byte = 1;
    
    if(pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }
    
    gettimeofday(&start, NULL);
    
    for(int i = 0; i < NUM_ITERATIONS; i++) {
        // Force context switch through pipe I/O
        write(pipefd[1], &byte, 1);
        read(pipefd[0], &byte, 1);
    }
    
    gettimeofday(&end, NULL);
    
    close(pipefd[0]);
    close(pipefd[1]);
    
    return ((end.tv_sec - start.tv_sec) * 1000000.0 +
            (end.tv_usec - start.tv_usec)) / NUM_ITERATIONS;
}

int main() {
    printf("Measuring context switch overhead...\n");
    double avg_time = measure_context_switch();
    printf("Average context switch time: %.2f microseconds\n", avg_time);
    
    return 0;
}