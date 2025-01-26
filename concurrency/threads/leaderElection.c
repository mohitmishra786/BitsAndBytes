#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define NUM_THREADS 5
#define HEARTBEAT_INTERVAL 1
#define HEARTBEAT_TIMEOUT 3

pthread_mutex_t lock;
int master_id = -1;
bool master_alive = false;
time_t last_heartbeat_time;

void* master_thread(void* arg) {
    int id = *((int*)arg);
    printf("Thread %d is now the master.\n", id);

    // Master thread sends periodic heartbeats
    for (int i = 0; i < 3; i++) { // Simulate master thread stopping after 3 heartbeats
        sleep(HEARTBEAT_INTERVAL); // Simulate heartbeat interval
        pthread_mutex_lock(&lock);
        last_heartbeat_time = time(NULL); // Update last heartbeat time
        pthread_mutex_unlock(&lock);
        printf("Master thread %d sent a heartbeat.\n", id);
    }

    // Simulate master thread dying
    pthread_mutex_lock(&lock);
    master_alive = false;
    pthread_mutex_unlock(&lock);
    printf("Master thread %d is stopping.\n", id);

    return NULL;
}

void* follower_thread(void* arg) {
    int id = *((int*)arg);

    while (1) {
        pthread_mutex_lock(&lock);
        time_t current_time = time(NULL);

        // Check if the master is alive
        if (master_alive && (current_time - last_heartbeat_time > HEARTBEAT_TIMEOUT)) {
            printf("Thread %d detected that the master is dead.\n", id);
            master_alive = false; // Master is considered dead
        }

        // If there is no master, elect a new one
        if (!master_alive) {
            master_id = id;
            master_alive = true;
            last_heartbeat_time = time(NULL); // Reset heartbeat time
            pthread_mutex_unlock(&lock);
            printf("Thread %d is promoting itself to master.\n", id);
            master_thread(arg); // Promote this thread to master
            break;
        }

        pthread_mutex_unlock(&lock);

        // Simulate follower thread doing work
        sleep(1);
        printf("Follower thread %d is waiting.\n", id);
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    pthread_mutex_init(&lock, NULL);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        if (i == 0) {
            // First thread is the initial master
            master_id = i;
            master_alive = true;
            last_heartbeat_time = time(NULL); // Initialize heartbeat time
            pthread_create(&threads[i], NULL, master_thread, &thread_ids[i]);
        } else {
            pthread_create(&threads[i], NULL, follower_thread, &thread_ids[i]);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    return 0;
}
