#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t readers_cond;
    pthread_cond_t writers_cond;
    int readers;
    int writers;
    int upgraders;
    bool is_writing;
} upgradeable_rwlock_t;

void upgradeable_rwlock_init(upgradeable_rwlock_t *lock) {
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->readers_cond, NULL);
    pthread_cond_init(&lock->writers_cond, NULL);
    lock->readers = 0;
    lock->writers = 0;
    lock->upgraders = 0;
    lock->is_writing = false;
}

void upgradeable_rwlock_read_lock(upgradeable_rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    while (lock->writers > 0 || lock->is_writing) {
        pthread_cond_wait(&lock->readers_cond, &lock->mutex);
    }
    lock->readers++;
    pthread_mutex_unlock(&lock->mutex);
}

void upgradeable_rwlock_read_unlock(upgradeable_rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->readers--;
    if (lock->readers == 0 && lock->writers > 0) {
        pthread_cond_signal(&lock->writers_cond);
    }
    pthread_mutex_unlock(&lock->mutex);
}

void upgradeable_rwlock_upgrade(upgradeable_rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->upgraders++;
    while (lock->readers > 1 || lock->writers > 0 || lock->is_writing) {
        pthread_cond_wait(&lock->writers_cond, &lock->mutex);
    }
    lock->readers--;
    lock->is_writing = true;
    lock->upgraders--;
    pthread_mutex_unlock(&lock->mutex);
}

void upgradeable_rwlock_write_unlock(upgradeable_rwlock_t *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->is_writing = false;
    if (lock->upgraders > 0) {
        pthread_cond_signal(&lock->writers_cond);
    } else if (lock->writers > 0) {
        pthread_cond_signal(&lock->writers_cond);
    } else {
        pthread_cond_broadcast(&lock->readers_cond);
    }
    pthread_mutex_unlock(&lock->mutex);
}

#define NUM_THREADS 5

upgradeable_rwlock_t rwlock;
int shared_data = 0;

void* thread_func(void* arg) {
    long id = (long)arg;
    while (1) {
        upgradeable_rwlock_read_lock(&rwlock);
        printf("Thread %ld read: %d\n", id, shared_data);
        
        if (rand() % 10 == 0) {  // 10% chance to upgrade
            printf("Thread %ld upgrading...\n", id);
            upgradeable_rwlock_upgrade(&rwlock);
            shared_data++;
            printf("Thread %ld upgraded and wrote: %d\n", id, shared_data);
            upgradeable_rwlock_write_unlock(&rwlock);
        } else {
            upgradeable_rwlock_read_unlock(&rwlock);
        }
        
        usleep(rand() % 100000);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    upgradeable_rwlock_init(&rwlock);

    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_func, (void*)i);
    }

    sleep(10);
    return 0;
}
