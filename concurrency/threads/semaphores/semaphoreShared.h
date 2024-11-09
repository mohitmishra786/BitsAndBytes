#ifndef SEMAPHORE_SHARED_H
#define SEMAPHORE_SHARED_H
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#define SHM_NAME "/my_shared_memory"
#define SEM_PRODUCER_NAME "/sem_producer"
#define SEM_CONSUMER_NAME "/sem_consumer"
#define MAX_MESSAGE_SIZE 256
struct shared_data {
  char message[MAX_MESSAGE_SIZE];
};
#endif
