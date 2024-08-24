
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <stdbool.h>

#define NUM_THREADS 4
#define ITERATIONS 1000000

// Shared data
int counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Thread function
void* thread_func(void* arg) {
    int thread_id = *(int*)arg;
    int local_counter = 0;

    // Pin thread to a specific CPU core
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(thread_id % sysconf(_SC_NPROCESSORS_ONLN), &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

    for (int i = 0; i < ITERATIONS; i++) {
        // Simulate some work
        for (int j = 0; j < 100; j++) {
            local_counter++;
        }

        // Update shared counter (atomic operation)
        __atomic_fetch_add(&counter, 1, __ATOMIC_SEQ_CST);

        // Periodically yield to allow other threads to run (simulating time-slicing)
        if (i % 1000 == 0) {
            sched_yield();
        }
    }

    printf("Thread %d finished. Local counter: %d\n", thread_id, local_counter);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    struct timespec start, end;

    // Start timing
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]) != 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // End timing
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate elapsed time
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("All threads finished.\n");
    printf("Final counter value: %d\n", __atomic_load_n(&counter, __ATOMIC_SEQ_CST));
    printf("Elapsed time: %.6f seconds\n", elapsed);

    return 0;
}
