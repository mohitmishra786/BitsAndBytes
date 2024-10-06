#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define NUM_INTERRUPTS 3
#define NUM_EVENTS 10

pthread_t interrupt_threads[NUM_INTERRUPTS];
pthread_mutex_t interrupt_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t interrupt_cond = PTHREAD_COND_INITIALIZER;
int interrupt_pending[NUM_INTERRUPTS] = {0};

void simulate_interrupt(int interrupt_id) {
    pthread_mutex_lock(&interrupt_mutex);
    interrupt_pending[interrupt_id] = 1;
    pthread_cond_signal(&interrupt_cond);
    pthread_mutex_unlock(&interrupt_mutex);
}

void *interrupt_handler(void *arg) {
    int interrupt_id = *(int*)arg;
    struct timespec start, end;
    
    while (1) {
        pthread_mutex_lock(&interrupt_mutex);
        while (!interrupt_pending[interrupt_id]) {
            pthread_cond_wait(&interrupt_cond, &interrupt_mutex);
        }
        interrupt_pending[interrupt_id] = 0;
        pthread_mutex_unlock(&interrupt_mutex);
        
        clock_gettime(CLOCK_MONOTONIC, &start);
        
        usleep(50000);  // 50ms
        
        clock_gettime(CLOCK_MONOTONIC, &end);
        long long duration = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
        
        printf("Interrupt %d handled. Duration: %lld ns\n", interrupt_id, duration);
    }
    
    return NULL;
}

int main() {
    int interrupt_ids[NUM_INTERRUPTS];
    
    for (int i = 0; i < NUM_INTERRUPTS; i++) {
        interrupt_ids[i] = i;
        pthread_create(&interrupt_threads[i], NULL, interrupt_handler, &interrupt_ids[i]);
    }
    
    for (int i = 0; i < NUM_EVENTS; i++) {
        int interrupt_id = i % NUM_INTERRUPTS;
        simulate_interrupt(interrupt_id);
        usleep(10000);  // 10ms between interrupts
    }
    
    sleep(1);  // Allow time for all interrupts to be handled
    
    return 0;
}
