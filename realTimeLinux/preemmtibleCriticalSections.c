#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>

#define NUM_THREADS 5
#define CRITICAL_SECTION_TIME 100000 // 100ms

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_function(void *arg) {
    int thread_id = *(int*)arg;
    struct timespec start, end;
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    pthread_mutex_lock(&mutex);
    printf("Thread %d entered critical section\n", thread_id);
    
    for (int i = 0; i < 10; i++) {
        usleep(CRITICAL_SECTION_TIME / 10);
        sched_yield();  // Simulate preemption point
        printf("Thread %d in critical section, iteration %d\n", thread_id, i);
    }
    
    pthread_mutex_unlock(&mutex);
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    long long duration = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
    
    printf("Thread %d exited critical section. Total time: %lld ns\n", thread_id, duration);
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}
