#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct rt_mutex {
    pthread_mutex_t lock;
    int owner_priority;
    int highest_waiter_priority;
};

struct task_struct {
    int id;
    int priority;
    struct rt_mutex *blocked_on;
};

#define NUM_TASKS 3
#define LOW_PRIO 1
#define MED_PRIO 2
#define HIGH_PRIO 3

struct rt_mutex mutex;
struct task_struct tasks[NUM_TASKS];

void rt_mutex_init(struct rt_mutex *mutex) {
    pthread_mutex_init(&mutex->lock, NULL);
    mutex->owner_priority = 0;
    mutex->highest_waiter_priority = 0;
}

void rt_mutex_lock(struct rt_mutex *mutex, struct task_struct *task) {
    printf("Task %d (priority %d) trying to acquire mutex\n", task->id, task->priority);
    
    if (mutex->owner_priority != 0 && task->priority > mutex->highest_waiter_priority) {
        mutex->highest_waiter_priority = task->priority;
        printf("Boosting mutex owner priority to %d\n", mutex->highest_waiter_priority);
    }
    
    pthread_mutex_lock(&mutex->lock);
    
    mutex->owner_priority = task->priority;
    mutex->highest_waiter_priority = 0;
    printf("Task %d acquired mutex\n", task->id);
}

void rt_mutex_unlock(struct rt_mutex *mutex, struct task_struct *task) {
    printf("Task %d releasing mutex\n", task->id);
    mutex->owner_priority = 0;
    pthread_mutex_unlock(&mutex->lock);
}

void* task_function(void *arg) {
    struct task_struct *task = (struct task_struct*)arg;
    
    printf("Task %d (priority %d) started\n", task->id, task->priority);
    sleep(1);
    rt_mutex_lock(&mutex, task);
    
    printf("Task %d working with mutex\n", task->id);
    sleep(2);
    
    rt_mutex_unlock(&mutex, task);
    
    printf("Task %d finished\n", task->id);
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_TASKS];
    
    rt_mutex_init(&mutex);
    
    tasks[0] = (struct task_struct){.id = 1, .priority = LOW_PRIO};
    tasks[1] = (struct task_struct){.id = 2, .priority = MED_PRIO};
    tasks[2] = (struct task_struct){.id = 3, .priority = HIGH_PRIO};
    
    for (int i = 0; i < NUM_TASKS; i++) {
        pthread_create(&threads[i], NULL, task_function, &tasks[i]);
        sleep(1);  // Start tasks with a slight delay
    }
    
    for (int i = 0; i < NUM_TASKS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}
