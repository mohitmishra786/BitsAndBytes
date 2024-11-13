#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define ARRAY_SIZE 1000000
#define NUM_THREADS 8

void parallel_array_operation(float* input, float* output, int size) {
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int thread_id = omp_get_thread_num();
        int chunk_size = size / NUM_THREADS;
        int start = thread_id * chunk_size;
        int end = (thread_id == NUM_THREADS - 1) ? size : start + chunk_size;

        // Simulate GPU-like parallel processing
        for (int i = start; i < end; i++) {
            // Simple arithmetic operation
            output[i] = input[i] * 2.0f + 1.0f;
        }
    }
}

int main() {
    float *input = (float*)malloc(ARRAY_SIZE * sizeof(float));
    float *output = (float*)malloc(ARRAY_SIZE * sizeof(float));
    
    // Initialize input array
    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        input[i] = (float)rand() / RAND_MAX;
    }

    // Measure execution time
    double start_time = omp_get_wtime();
    parallel_array_operation(input, output, ARRAY_SIZE);
    double end_time = omp_get_wtime();

    printf("Execution time: %f seconds\n", end_time - start_time);
    printf("First 5 results: %f %f %f %f %f\n", 
           output[0], output[1], output[2], output[3], output[4]);

    free(input);
    free(output);
    return 0;
}