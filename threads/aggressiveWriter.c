#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t readers_cond;
    pthread_cond_t writers_cond;
    int active_readers;
    int waiting_readers;
    int active_writers;
    int waiting_writers;
    bool writer_waiting;
} aggressive_write_pref_rwlock_t;

void aggressive_write_pref_rwlock_init(aggressive_write_pref_rwlock_t *lock) {
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->readers_cond, NULL);
    pthread_cond_init(&lock->writers_cond, NULL);
    lock->active_readers = 0;
    lock->waiting_readers = 0;
    lock->active_writers = 0;
    lock->waiting_writers = 0;
    lock->writer_waiting = false;
}

void aggressive_write_pref_rwlock_read_lock(aggressive_write_pref_rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    while (lock->active_writers > 0 || lock->writer_waiting) {
        lock->waiting_readers++;
        pthread_cond_wait(&lock->readers_cond, &lock->mutex);
        lock->waiting_readers--;
    }
    lock->active_readers++;
    pthread_mutex_unlock(&lock->mutex);
}

void aggressive_write_pref_rwlock_read_unlock(aggressive_write_pref_rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->active_readers--;
    if (lock->active_readers == 0 && lock->waiting_writers > 0) {
        lock->writer_waiting = true;
        pthread_cond_signal(&lock->writers_cond);
    }
    pthread_mutex_unlock(&lock->mutex);
}

void aggressive_write_pref_rwlock_write_lock(aggressive_write_pref_rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->waiting_writers++;
    while (lock->active_readers > 0 || lock->active_writers > 0) {
        lock->writer_waiting = true;
        pthread_cond_wait(&lock->writers_cond, &lock->mutex);
    }
    lock->waiting_writers--;
    lock->active_writers++;
    lock->writer_waiting = false;
    pthread_mutex_unlock(&lock->mutex);
}

void aggressive_write_pref_rwlock_write_unlock(aggressive_write_pref_rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->active_writers--;
    if (lock->waiting_writers > 0) {
        pthread_cond_signal(&lock->writers_cond);
    } else {
        pthread_cond_broadcast(&lock->readers_cond);
    }
    pthread_mutex_unlock(&lock->mutex);
}

#define NUM_READERS 5
#define NUM_WRITERS 2

aggressive_write_pref_rwlock_t rwlock;
int shared_data = 0;

void* reader(void* arg) {
    long id = (long)arg;
    while (1) {
        aggressive_write_pref_rwlock_read_lock(&rwlock);
        printf("Reader %ld read: %d\n", id, shared_data);
        aggressive_write_pref_rwlock_read_unlock(&rwlock);
        usleep(rand() % 100000);
    }
    return NULL;
}

void* writer(void* arg) {
    long id = (long)arg;
    while (1) {
        aggressive_write_pref_rwlock_write_lock(&rwlock);
        shared_data++;
        printf("Writer %ld wrote: %d\n", id, shared_data);
        aggressive_write_pref_rwlock_write_unlock(&rwlock);
        usleep(rand() % 200000);
    }
    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS], writers[NUM_WRITERS];
    aggressive_write_pref_rwlock_init(&rwlock);
    
    srand(time(NULL));  // Seed the random number generator

    for (long i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader, (void*)i);
    }
    for (long i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writers[i], NULL, writer, (void*)i);
    }

    sleep(30);

    return 0;
}
