#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_READERS 4
#define BUFFER_SIZE 1024

char buffer[BUFFER_SIZE];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* reader_thread(void* arg) {
    long thread_id = (long)arg;

    while (1) {
        pthread_mutex_lock(&lock);
        printf("Reader %ld: %s\n", thread_id, buffer);
        pthread_mutex_unlock(&lock);
        usleep(500000);
    }

    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS];

    for (long i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader_thread, (void*)i);
    }

    char* strings[] = {"Hello, World!", "Mutex is amazing", "C programming rocks"};
    int num_strings = sizeof(strings) / sizeof(strings[0]);
    int index = 0;

    while (1) {
        pthread_mutex_lock(&lock);
        snprintf(buffer, BUFFER_SIZE, "%s", strings[index]);
        pthread_mutex_unlock(&lock);

        index = (index + 1) % num_strings;
        sleep(2);
    }

    return 0;
}
