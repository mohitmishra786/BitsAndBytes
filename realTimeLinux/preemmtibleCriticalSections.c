#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>
#include <linux/futex.h>

typedef struct {
    int lock;
    int owner;
} rt_mutex_t;

int shared_counter = 0;
rt_mutex_t my_lock = {0, -1};

static long sys_futex(void *addr1, int op, int val1, struct timespec *timeout, void *addr2, int val3) {
    return syscall(SYS_futex, addr1, op, val1, timeout, addr2, val3);
}

void rt_mutex_init(rt_mutex_t *mutex) {
    mutex->lock = 0;
    mutex->owner = -1;
}

void rt_mutex_lock(rt_mutex_t *mutex) {
    int tid = (int)syscall(SYS_gettid);
    while (1) {
        int expected = 0;
        if (__sync_bool_compare_and_swap(&mutex->lock, expected, 1)) {
            mutex->owner = tid;
            break;
        }
        sys_futex(&mutex->lock, FUTEX_WAIT, 1, NULL, NULL, 0);
    }
}

void rt_mutex_unlock(rt_mutex_t *mutex) {
    mutex->owner = -1;
    mutex->lock = 0;
    sys_futex(&mutex->lock, FUTEX_WAKE, 1, NULL, NULL, 0);
}

void do_work(int id) {
    usleep(rand() % 1000);
    printf("Thread %d: Working...\n", id);
}

void critical_section(int id) {
    printf("Thread %d: Entering critical section\n", id);
    rt_mutex_lock(&my_lock);

    printf("Thread %d: Inside critical section\n", id);
    shared_counter++;
    do_work(id);

    printf("Thread %d: Preemption point (cond_resched)\n", id);
    sched_yield();

    do_work(id);
    printf("Thread %d: Shared counter value: %d\n", id, shared_counter);

    rt_mutex_unlock(&my_lock);
    printf("Thread %d: Exiting critical section\n", id);
}

void* thread_func(void* arg) {
    int id = *(int*)arg;
    printf("Thread %d: Started\n", id);

    for (int i = 0; i < 3; i++) {
        critical_section(id);
        usleep(rand() % 2000);
    }

    printf("Thread %d: Finished\n", id);
    return NULL;
}

int main() {
    pthread_t threads[5];
    int thread_ids[5];

    printf("Main: Initializing RT-mutex\n");
    rt_mutex_init(&my_lock);

    printf("Main: Creating threads\n");
    for (int i = 0; i < 5; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }

    printf("Main: Waiting for threads to finish\n");
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Main: All threads finished. Final shared counter value: %d\n", shared_counter);
    return 0;
}
