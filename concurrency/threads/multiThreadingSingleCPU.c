#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define NUM_THREADS 4
#define CACHE_SIZE 256
#define CACHE_LINE_SIZE 64
#define NUM_CACHE_LINES (CACHE_SIZE / CACHE_LINE_SIZE)
#define MAX_ITERATIONS 1000

typedef enum {
    MODIFIED,
    EXCLUSIVE,
    SHARED,
    INVALID
} CacheLineState;

typedef struct {
    int data;
    CacheLineState state;
} CacheLine;

typedef struct {
    CacheLine lines[NUM_CACHE_LINES];
} Cache;

typedef struct {
    int id;
    Cache cache;
    pthread_t thread;
    bool is_running;
    int iterations;
} Thread;

Thread threads[NUM_THREADS];
pthread_mutex_t scheduler_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t scheduler_cond = PTHREAD_COND_INITIALIZER;
bool simulation_running = true;

void init_cache(Cache *cache) {
    for (int i = 0; i < NUM_CACHE_LINES; i++) {
        cache->lines[i].data = 0;
        cache->lines[i].state = INVALID;
    }
}

void cache_read(Thread *thread, int address) {
    int cache_line = address / CACHE_LINE_SIZE;
    if (thread->cache.lines[cache_line].state == INVALID) {
        // Simulate cache miss and memory fetch
        thread->cache.lines[cache_line].data = address;
        thread->cache.lines[cache_line].state = SHARED;
        printf("[%d] Thread %d: Cache miss, fetched data for address %d\n", thread->iterations, thread->id, address);
    } else {
        printf("[%d] Thread %d: Cache hit, read data from address %d\n", thread->iterations, thread->id, address);
    }
}

void cache_write(Thread *thread, int address, int value) {
    int cache_line = address / CACHE_LINE_SIZE;
    thread->cache.lines[cache_line].data = value;
    thread->cache.lines[cache_line].state = MODIFIED;
    printf("[%d] Thread %d: Wrote value %d to address %d\n", thread->iterations, thread->id, value, address);
}

void flush_cache(Thread *thread) {
    for (int i = 0; i < NUM_CACHE_LINES; i++) {
        if (thread->cache.lines[i].state == MODIFIED) {
            // Simulate writing back to main memory
            printf("[%d] Thread %d: Flushing modified cache line %d\n", thread->iterations, thread->id, i);
            thread->cache.lines[i].state = INVALID;
        }
    }
}

void *thread_function(void *arg) {
    Thread *thread = (Thread *)arg;
    while (simulation_running) {
        // Simulate some work
        for (int i = 0; i < 5; i++) {
            int address = rand() % CACHE_SIZE;
            if (rand() % 2 == 0) {
                cache_read(thread, address);
            } else {
                cache_write(thread, address, rand() % 100);
            }
            usleep(10000);  // Sleep for 10ms
        }

        thread->iterations++;

        // Simulate context switch
        pthread_mutex_lock(&scheduler_mutex);
        flush_cache(thread);
        thread->is_running = false;
        printf("[%d] Thread %d: Yielding CPU\n", thread->iterations, thread->id);
        pthread_cond_signal(&scheduler_cond);
        pthread_mutex_unlock(&scheduler_mutex);

        // Wait to be scheduled again
        pthread_mutex_lock(&scheduler_mutex);
        while (!thread->is_running && simulation_running) {
            pthread_cond_wait(&scheduler_cond, &scheduler_mutex);
        }
        pthread_mutex_unlock(&scheduler_mutex);
    }
    return NULL;
}

void scheduler() {
    int current_thread = 0;
    while (simulation_running) {
        pthread_mutex_lock(&scheduler_mutex);
        threads[current_thread].is_running = true;
        printf("Scheduler: Activating thread %d\n", current_thread);
        pthread_cond_signal(&scheduler_cond);
        pthread_mutex_unlock(&scheduler_mutex);

        current_thread = (current_thread + 1) % NUM_THREADS;
        usleep(100000);  // Sleep for 100ms before next scheduling decision
    }
}

int main() {
    srand(time(NULL));

    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].id = i;
        threads[i].iterations = 0;
        init_cache(&threads[i].cache);
        threads[i].is_running = false;
        pthread_create(&threads[i].thread, NULL, thread_function, &threads[i]);
    }

    pthread_t scheduler_thread;
    pthread_create(&scheduler_thread, NULL, (void *(*)(void *))scheduler, NULL);

    // Run simulation for a set time
    sleep(10);  // Run for 10 seconds

    // Stop simulation
    simulation_running = false;
    pthread_mutex_lock(&scheduler_mutex);
    pthread_cond_broadcast(&scheduler_cond);
    pthread_mutex_unlock(&scheduler_mutex);

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i].thread, NULL);
        printf("Thread %d completed %d iterations\n", i, threads[i].iterations);
    }

    pthread_join(scheduler_thread, NULL);

    return 0;
}
