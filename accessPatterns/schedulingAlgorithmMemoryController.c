#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 4
#define MEMORY_INTENSITY_THRESHOLD 10
#define NUM_BANKS 8
#define BATCH_SIZE 4

typedef struct {
    int id;
    int mrpki;
    int priority;
    int requests_served;
} thread_t;

typedef struct {
    int bank_id;
    int request_count;
    int is_marked;
} bank_t;

// First Ready-First Come First Serve (FR-FCFS)
void frFcfsScheduleRequest(thread_t threads[], int num_threads, bank_t banks[], int num_banks) {
    // Check the row buffer hit queue first
    for (int i = 0; i < num_banks; i++) {
        if (banks[i].is_marked) {
            // Schedule the oldest request in the hit queue
            banks[i].is_marked = 0;
            banks[i].request_count--;
            printf("FR-FCFS scheduled request from bank %d\n", banks[i].bank_id);
            return;
        }
    }

    // No hits available, schedule the oldest request from the miss queue
    for (int i = 0; i < num_banks; i++) {
        if (banks[i].request_count > 0) {
            banks[i].request_count--;
            printf("FR-FCFS scheduled request from bank %d\n", banks[i].bank_id);
            return;
        }
    }
}

// Parallelism-Aware Batch Scheduling (PAR-BS)
void parBsScheduleRequest(thread_t threads[], int num_threads, bank_t banks[], int num_banks) {
    // Group requests into batches
    bank_t batch[BATCH_SIZE];
    int batch_size = 0;

    // Rank banks based on the number of requests
    for (int i = 0; i < num_banks; i++) {
        if (banks[i].request_count > 0) {
            batch[batch_size++] = banks[i];
        }
    }

    // Sort the batch by request count in descending order
    for (int i = 0; i < batch_size - 1; i++) {
        for (int j = 0; j < batch_size - i - 1; j++) {
            if (batch[j].request_count < batch[j + 1].request_count) {
                bank_t temp = batch[j];
                batch[j] = batch[j + 1];
                batch[j + 1] = temp;
            }
        }
    }

    // Schedule requests to maximize bank-level parallelism
    for (int i = 0; i < batch_size; i++) {
        if (batch[i].is_marked) {
            // Schedule a row buffer hit
            batch[i].is_marked = 0;
            batch[i].request_count--;
            printf("PAR-BS scheduled row buffer hit request from bank %d\n", batch[i].bank_id);
        } else if (batch[i].request_count > 0) {
            // Schedule the oldest request
            batch[i].request_count--;
            printf("PAR-BS scheduled request from bank %d\n", batch[i].bank_id);
        }
    }
}

// Adaptive Thread-Aware Scheduling
void classifyThreads(thread_t threads[], int num_threads) {
    for (int i = 0; i < num_threads; i++) {
        if (threads[i].mrpki >= MEMORY_INTENSITY_THRESHOLD) {
            threads[i].priority = 1;
        } else {
            threads[i].priority = 0;
        }
    }
}

void adaptSchedulingPolicy(thread_t threads[], int num_threads) {
    for (int i = 0; i < num_threads; i++) {
        if (threads[i].priority == 1) {
            threads[i].priority++;
        }
    }
}

int adaptiveScheduleRequest(thread_t threads[], int num_threads, bank_t banks[], int num_banks) {
    int highest_priority = -1;
    int oldest_thread = -1;
    for (int i = 0; i < num_threads; i++) {
        if (threads[i].priority > highest_priority) {
            highest_priority = threads[i].priority;
            oldest_thread = i;
        } else if (threads[i].priority == highest_priority && threads[i].requests_served < threads[oldest_thread].requests_served) {
            oldest_thread = i;
        }
    }
    threads[oldest_thread].requests_served++;
    return oldest_thread;
}

int main() {
    thread_t threads[NUM_THREADS];
    bank_t banks[NUM_BANKS];

    // Initialize threads and banks
    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].id = i;
        threads[i].mrpki = rand() % 20;
        threads[i].priority = 0;
        threads[i].requests_served = 0;
    }

    for (int i = 0; i < NUM_BANKS; i++) {
        banks[i].bank_id = i;
        banks[i].request_count = 0;
        banks[i].is_marked = 0;
    }

    int total_cycles = 1000;
    for (int cycle = 0; cycle < total_cycles; cycle++) {
        // Simulate thread behavior and memory requests
        for (int i = 0; i < NUM_THREADS; i++) {
            threads[i].mrpki = rand() % 20;
            banks[rand() % NUM_BANKS].request_count++;
            banks[rand() % NUM_BANKS].is_marked = 1;
        }

        // FR-FCFS scheduling
        printf("FR-FCFS scheduling:\n");
        frFcfsScheduleRequest(threads, NUM_THREADS, banks, NUM_BANKS);

        // PAR-BS scheduling
        printf("PAR-BS scheduling:\n");
        parBsScheduleRequest(threads, NUM_THREADS, banks, NUM_BANKS);

        // Adaptive Thread-Aware Scheduling
        if (cycle % 100 == 0) {
            printf("Adaptive Thread-Aware Scheduling:\n");
            classifyThreads(threads, NUM_THREADS);
            adaptSchedulingPolicy(threads, NUM_THREADS);
        }
        int scheduled_thread = adaptiveScheduleRequest(threads, NUM_THREADS, banks, NUM_BANKS);
        printf("Scheduled request from thread %d\n", scheduled_thread);
    }

    return 0;
}
