#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

#define NUM_THREADS 4
#define NUM_ITERATIONS 1000000

// Shared variables
atomic_int counter = 0;
int data = 0;

// Thread function
void* thread_function(void* arg) {
    int thread_id = *(int*)arg;
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        // Atomically increment the counter
        atomic_fetch_add(&counter, 1);
        
        // Perform a non-atomic operation on data
        data++;
        
        // Use a memory barrier to ensure the increment is visible to other threads
        atomic_thread_fence(memory_order_release);
        
        // Read the counter value
        int current_counter = atomic_load(&counter);
        
        // Use another memory barrier before accessing data
        atomic_thread_fence(memory_order_acquire);
        
        // Check if data matches the counter
        if (data != current_counter) {
            printf("Thread %d: Inconsistency detected! Counter: %d, Data: %d\n",
                   thread_id, current_counter, data);
        }
    }
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    
    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]) != 0) {
            perror("pthread_create");
            exit(1);
        }
    }
    
    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            exit(1);
        }
    }
    
    printf("Final counter value: %d\n", atomic_load(&counter));
    printf("Final data value: %d\n", data);
    
    return 0;
}
