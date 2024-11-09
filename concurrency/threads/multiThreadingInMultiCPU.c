#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

#define NUM_CPUS 4
#define NUM_THREADS 8
#define MAX_LOAD 100

typedef struct {
    int id;
    int last_cpu;
    int priority;
    bool is_running;
} Thread;

typedef struct {
    int id;
    int current_load;
    pthread_mutex_t lock;
} CPU;

Thread threads[NUM_THREADS];
CPU cpus[NUM_CPUS];
pthread_mutex_t scheduler_mutex = PTHREAD_MUTEX_INITIALIZER;
bool running = true; // Global flag to control the loop

void init_system() {
    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].id = i;
        threads[i].last_cpu = rand() % NUM_CPUS;
        threads[i].priority = rand() % 10;
        threads[i].is_running = false;
    }

    for (int i = 0; i < NUM_CPUS; i++) {
        cpus[i].id = i;
        cpus[i].current_load = 0;
        pthread_mutex_init(&cpus[i].lock, NULL);
    }
}

int select_cpu(Thread* thread) {
    int selected_cpu = -1;
    int min_load = INT_MAX;

    // Check affinity first
    if (cpus[thread->last_cpu].current_load < MAX_LOAD) {
        return thread->last_cpu;
    }

    // Find CPU with minimum load
    for (int i = 0; i < NUM_CPUS; i++) {
        if (cpus[i].current_load < min_load) {
            min_load = cpus[i].current_load;
            selected_cpu = i;
        }
    }

    return selected_cpu;
}

void* cpu_thread(void* arg) {
    int cpu_id = *(int*)arg;
    while (running) {  // Check the global running flag
        pthread_mutex_lock(&scheduler_mutex);
        for (int i = 0; i < NUM_THREADS; i++) {
            if (!threads[i].is_running) {
                int selected_cpu = select_cpu(&threads[i]);
                if (selected_cpu == cpu_id) {
                    pthread_mutex_lock(&cpus[cpu_id].lock);
                    cpus[cpu_id].current_load += threads[i].priority;
                    pthread_mutex_unlock(&cpus[cpu_id].lock);
                    threads[i].is_running = true;
                    threads[i].last_cpu = cpu_id;
                    printf("Thread %d scheduled on CPU %d\n", threads[i].id, cpu_id);
                }
            }
        }
        pthread_mutex_unlock(&scheduler_mutex);

        // Simulate work
        usleep(100000);  // 100ms

        pthread_mutex_lock(&scheduler_mutex);
        pthread_mutex_lock(&cpus[cpu_id].lock);
        cpus[cpu_id].current_load = (cpus[cpu_id].current_load > 0) ? cpus[cpu_id].current_load - 1 : 0;
        pthread_mutex_unlock(&cpus[cpu_id].lock);
        for (int i = 0; i < NUM_THREADS; i++) {
            if (threads[i].is_running && threads[i].last_cpu == cpu_id) {
                threads[i].is_running = false;
                printf("Thread %d finished on CPU %d\n", threads[i].id, cpu_id);
            }
        }
        pthread_mutex_unlock(&scheduler_mutex);
    }
    return NULL;
}

int main() {
    init_system();

    pthread_t cpu_threads[NUM_CPUS];
    int cpu_ids[NUM_CPUS];

    for (int i = 0; i < NUM_CPUS; i++) {
        cpu_ids[i] = i;
        pthread_create(&cpu_threads[i], NULL, cpu_thread, &cpu_ids[i]);
    }

    // Wait for 10 seconds
    sleep(10);

    // Set the running flag to false to stop the threads
    running = false;

    for (int i = 0; i < NUM_CPUS; i++) {
        pthread_join(cpu_threads[i], NULL);
    }

    printf("Program finished after 10 seconds.\n");
    return 0;
}
