#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define NUM_READERS 5
#define NUM_WRITERS 2
#define BUFFER_SIZE 10
#define NUM_ITERATIONS 20
#define ENABLE_DETAILED_OUTPUT 1

// Read-Write Lock structure
typedef struct {
    pthread_mutex_t counter_lock;
    pthread_mutex_t write_lock;
    int read_count;
} rwlock_t;

// Bounded Buffer structure
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
    int *buffer;
    int size;
    int count;
    int in;
    int out;
} bounded_buffer_t;

// Thread arguments structure
typedef struct {
    rwlock_t *lock;
    int *shared_data;
    bounded_buffer_t *buffer;
    int thread_id;
} thread_args_t;

void rwlock_init(rwlock_t *lock);
void rwlock_destroy(rwlock_t *lock);
void rwlock_read_lock(rwlock_t *lock);
void rwlock_read_unlock(rwlock_t *lock);
void rwlock_write_lock(rwlock_t *lock);
void rwlock_write_unlock(rwlock_t *lock);
void bb_init(bounded_buffer_t *bb, int size);
void bb_destroy(bounded_buffer_t *bb);
void bb_produce(bounded_buffer_t *bb, int item);
int bb_consume(bounded_buffer_t *bb);
void* reader_thread(void* arg);
void* writer_thread(void* arg);

// Global flag for clean shutdown
volatile sig_atomic_t running = 1;

// Signal handler for clean shutdown
void handle_signal(int sig) {
    running = 0;
}

// Read-Write Lock Implementation
void rwlock_init(rwlock_t *lock) {
    pthread_mutex_init(&lock->counter_lock, NULL);
    pthread_mutex_init(&lock->write_lock, NULL);
    lock->read_count = 0;
}

void rwlock_destroy(rwlock_t *lock) {
    pthread_mutex_destroy(&lock->counter_lock);
    pthread_mutex_destroy(&lock->write_lock);
}

void rwlock_read_lock(rwlock_t *lock) {
    pthread_mutex_lock(&lock->counter_lock);
    lock->read_count++;
    if (lock->read_count == 1) {
        pthread_mutex_lock(&lock->write_lock);
    }
    pthread_mutex_unlock(&lock->counter_lock);
}

void rwlock_read_unlock(rwlock_t *lock) {
    pthread_mutex_lock(&lock->counter_lock);
    lock->read_count--;
    if (lock->read_count == 0) {
        pthread_mutex_unlock(&lock->write_lock);
    }
    pthread_mutex_unlock(&lock->counter_lock);
}

void rwlock_write_lock(rwlock_t *lock) {
    pthread_mutex_lock(&lock->write_lock);
}

void rwlock_write_unlock(rwlock_t *lock) {
    pthread_mutex_unlock(&lock->write_lock);
}

void bb_init(bounded_buffer_t *bb, int size) {
    pthread_mutex_init(&bb->mutex, NULL);
    pthread_cond_init(&bb->not_full, NULL);
    pthread_cond_init(&bb->not_empty, NULL);
    bb->buffer = malloc(size * sizeof(int));
    bb->size = size;
    bb->count = 0;
    bb->in = 0;
    bb->out = 0;
}

void bb_destroy(bounded_buffer_t *bb) {
    pthread_mutex_destroy(&bb->mutex);
    pthread_cond_destroy(&bb->not_full);
    pthread_cond_destroy(&bb->not_empty);
    free(bb->buffer);
}

void bb_produce(bounded_buffer_t *bb, int item) {
    pthread_mutex_lock(&bb->mutex);
    
    while (bb->count == bb->size && running) {
        pthread_cond_wait(&bb->not_full, &bb->mutex);
    }
    
    if (!running) {
        pthread_mutex_unlock(&bb->mutex);
        return;
    }
    
    bb->buffer[bb->in] = item;
    bb->in = (bb->in + 1) % bb->size;
    bb->count++;
    
    pthread_cond_signal(&bb->not_empty);
    pthread_mutex_unlock(&bb->mutex);
}

int bb_consume(bounded_buffer_t *bb) {
    pthread_mutex_lock(&bb->mutex);
    
    while (bb->count == 0 && running) {
        pthread_cond_wait(&bb->not_empty, &bb->mutex);
    }
    
    if (!running) {
        pthread_mutex_unlock(&bb->mutex);
        return -1;
    }
    
    int item = bb->buffer[bb->out];
    bb->out = (bb->out + 1) % bb->size;
    bb->count--;
    
    pthread_cond_signal(&bb->not_full);
    pthread_mutex_unlock(&bb->mutex);
    
    return item;
}

void* reader_thread(void* arg) {
    thread_args_t *args = (thread_args_t*)arg;
    
    for (int i = 0; i < NUM_ITERATIONS && running; i++) {
        rwlock_read_lock(args->lock);
        if (ENABLE_DETAILED_OUTPUT) {
            printf("\033[0;32mReader %d read value: %d\033[0m\n", 
                   args->thread_id, *args->shared_data);
        }
        rwlock_read_unlock(args->lock);
        
        int item = bb_consume(args->buffer);
        if (item != -1 && ENABLE_DETAILED_OUTPUT) {
            printf("\033[0;32mReader %d consumed: %d\033[0m\n", 
                   args->thread_id, item);
        }
        
        usleep(rand() % 1000);
    }
    
    return NULL;
}

void* writer_thread(void* arg) {
    thread_args_t *args = (thread_args_t*)arg;
    
    for (int i = 0; i < NUM_ITERATIONS && running; i++) {
        int new_value = rand() % 100;
        
        rwlock_write_lock(args->lock);
        *args->shared_data = new_value;
        if (ENABLE_DETAILED_OUTPUT) {
            printf("\033[0;31mWriter %d wrote value: %d\033[0m\n", 
                   args->thread_id, new_value);
        }
        rwlock_write_unlock(args->lock);
        
        bb_produce(args->buffer, new_value);
        if (ENABLE_DETAILED_OUTPUT) {
            printf("\033[0;31mWriter %d produced: %d\033[0m\n", 
                   args->thread_id, new_value);
        }
        
        usleep(rand() % 2000);
    }
    
    return NULL;
}

int main() {
    // Setup signal handler for clean shutdown
    signal(SIGINT, handle_signal);
    
    // Initialize random seed
    srand(time(NULL));
    
    // Initialize shared resources
    rwlock_t lock;
    rwlock_init(&lock);
    
    bounded_buffer_t buffer;
    bb_init(&buffer, BUFFER_SIZE);
    
    int shared_data = 0;
    
    printf("Starting simulation with:\n");
    printf("- %d readers\n", NUM_READERS);
    printf("- %d writers\n", NUM_WRITERS);
    printf("- Buffer size: %d\n", BUFFER_SIZE);
    printf("- Iterations: %d\n\n", NUM_ITERATIONS);
    
    // Create threads
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];
    thread_args_t reader_args[NUM_READERS];
    thread_args_t writer_args[NUM_WRITERS];
    
    // Start readers
    for (int i = 0; i < NUM_READERS; i++) {
        reader_args[i].lock = &lock;
        reader_args[i].shared_data = &shared_data;
        reader_args[i].buffer = &buffer;
        reader_args[i].thread_id = i;
        pthread_create(&readers[i], NULL, reader_thread, &reader_args[i]);
    }
    
    // Start writers
    for (int i = 0; i < NUM_WRITERS; i++) {
        writer_args[i].lock = &lock;
        writer_args[i].shared_data = &shared_data;
        writer_args[i].buffer = &buffer;
        writer_args[i].thread_id = i;
        pthread_create(&writers[i], NULL, writer_thread, &writer_args[i]);
    }
    
    // Wait for threads to finish
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }
    
    // Cleanup
    bb_destroy(&buffer);
    rwlock_destroy(&lock);
    
    printf("\nSimulation completed successfully!\n");
    return 0;
}