#include <stdio.h>    // For NULL definition
#include <pthread.h>  // For pthread functions and types

void *newthread(void *arg) {
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t thread;
    pthread_create(&thread, NULL, &newthread, NULL);
    pthread_join(thread, NULL);
    return 0;
}