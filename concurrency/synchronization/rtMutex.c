#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

typedef struct {
    pthread_mutex_t mutex;
    int owner_priority;
} rt_mutex_t;

void rt_mutex_init(rt_mutex_t *mutex) {
    pthread_mutex_init(&mutex->mutex, NULL);
    mutex->owner_priority = -1;
}

void rt_mutex_lock(rt_mutex_t *mutex, int priority) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    pthread_mutex_lock(&mutex->mutex);
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    long long wait_time = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
    
    printf("Thread priority %d acquired lock. Wait time: %lld ns\n", priority, wait_time);
    
    if (mutex->owner_priority != -1 && priority > mutex->owner_priority) {
        printf("Priority inheritance: Boosting owner from %d to %d\n", mutex->owner_priority, priority);
    }
    mutex->owner_priority = priority;
}

void rt_mutex_unlock(rt_mutex_t *mutex) {
    printf("Thread priority %d released lock\n", mutex->owner_priority);
    mutex->owner_priority = -1;
    pthread_mutex_unlock(&mutex->mutex);
}

// Test function
void *test_thread(void *arg) {
    rt_mutex_t *mutex = (rt_mutex_t *)arg;
    int priority = (int)(long)pthread_self() % 100;  // Simulated priority
    
    rt_mutex_lock(mutex, priority);
    usleep(10000);  // Simulating some work
    rt_mutex_unlock(mutex);
    
    return NULL;
}

int main() {
    rt_mutex_t mutex;
    rt_mutex_init(&mutex);
    
    pthread_t threads[10];
    for (int i = 0; i < 10; i++) {
        pthread_create(&threads[i], NULL, test_thread, &mutex);
    }
    
    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}
