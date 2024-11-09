#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define NUM_READERS 4
#define NUM_WRITERS 2
#define BUFFER_SIZE 1024

char buffer[BUFFER_SIZE];
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void* reader_thread(void* arg) {
    long thread_id = (long)arg;
    
    while (1) {
        pthread_rwlock_rdlock(&rwlock);
        printf("Reader %ld: %s\n", thread_id, buffer);
        pthread_rwlock_unlock(&rwlock);
        usleep(100000);  // Sleep for 0.1 seconds
    }
    
    return NULL;
}

void* writer_thread(void* arg) {
    long thread_id = (long)arg;
    char local_buffer[BUFFER_SIZE];
    
    while (1) {
        snprintf(local_buffer, BUFFER_SIZE, "Writer %ld: %ld", thread_id, random());
        
        pthread_rwlock_wrlock(&rwlock);
        strcpy(buffer, local_buffer);
        pthread_rwlock_unlock(&rwlock);
        
        usleep(500000);
    }
    
    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];
    
    for (long i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader_thread, (void*)i);
    }
    
    for (long i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&writers[i], NULL, writer_thread, (void*)i);
    }
    
    sleep(10);
    
    return 0;
}
