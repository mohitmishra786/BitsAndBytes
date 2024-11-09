#include "semaphoreShared.h"

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(shm_fd, sizeof(struct shared_data)) == -1) {
        perror("ftruncate");
        exit(1);
    }

    struct shared_data *shared_mem = mmap(NULL, sizeof(struct shared_data),
                                          PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    sem_t *sem_prod = sem_open(SEM_PRODUCER_NAME, O_CREAT, 0666, 0);
    sem_t *sem_cons = sem_open(SEM_CONSUMER_NAME, O_CREAT, 0666, 1);
    if (sem_prod == SEM_FAILED || sem_cons == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    for (int i = 0; i < 10; i++) {
        sem_wait(sem_prod);
        printf("Consumed: %s\n", shared_mem->message);
        memset(shared_mem->message, 0, MAX_MESSAGE_SIZE);
        sem_post(sem_cons);
        sleep(1); 
    }

    sem_close(sem_prod);
    sem_close(sem_cons);
    sem_unlink(SEM_PRODUCER_NAME);
    sem_unlink(SEM_CONSUMER_NAME);
    munmap(shared_mem, sizeof(struct shared_data));
    close(shm_fd);
    shm_unlink(SHM_NAME);

    return 0;
}
