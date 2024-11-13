#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define WARP_SIZE 32
#define NUM_WARPS 4
#define TOTAL_THREADS (WARP_SIZE * NUM_WARPS)

typedef struct {
    int thread_id;
    float* input;
    float* output;
    int size;
} ThreadData;

void* simt_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int tid = data->thread_id;
    int stride = TOTAL_THREADS;
    
    // Simulate SIMT execution with divergent paths
    for (int i = tid; i < data->size; i += stride) {
        if (data->input[i] > 0.5f) {
            // Branch 1: Square the value
            data->output[i] = data->input[i] * data->input[i];
        } else {
            // Branch 2: Double the value
            data->output[i] = data->input[i] * 2.0f;
        }
    }
    
    return NULL;
}

int main() {
    const int size = 1024;
    float* input = (float*)malloc(size * sizeof(float));
    float* output = (float*)malloc(size * sizeof(float));
    pthread_t threads[TOTAL_THREADS];
    ThreadData thread_data[TOTAL_THREADS];
    
    // Initialize input array
    srand(time(NULL));
    for (int i = 0; i < size; i++) {
        input[i] = (float)rand() / RAND_MAX;
    }
    
    // Create threads
    for (int i = 0; i < TOTAL_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].input = input;
        thread_data[i].output = output;
        thread_data[i].size = size;
        pthread_create(&threads[i], NULL, simt_thread, &thread_data[i]);
    }
    
    // Join threads
    for (int i = 0; i < TOTAL_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Print sample results
    printf("Sample results:\n");
    for (int i = 0; i < 5; i++) {
        printf("Input: %.4f, Output: %.4f\n", input[i], output[i]);
    }
    
    free(input);
    free(output);
    return 0;
}