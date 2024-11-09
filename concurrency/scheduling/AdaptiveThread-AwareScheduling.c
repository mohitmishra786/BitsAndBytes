#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_THREADS 8
#define MAX_QUEUE_SIZE 100
#define MEMORY_INTENSIVE_THRESHOLD 10
#define ADAPTATION_INTERVAL 1000

typedef struct {
    int address;
    time_t timestamp;
    int thread_id;
} MemoryRequest;

typedef struct {
    MemoryRequest requests[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
} Queue;

typedef struct {
    int memory_requests;
    int instructions;
    bool is_memory_intensive;
    int priority;
} ThreadInfo;

void initQueue(Queue *q) {
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

bool isQueueEmpty(Queue *q) {
    return q->size == 0;
}

bool isQueueFull(Queue *q) {
    return q->size == MAX_QUEUE_SIZE;
}

void enqueue(Queue *q, MemoryRequest request) {
    if (isQueueFull(q)) {
        printf("Queue is full, cannot enqueue\n");
        return;
    }
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->requests[q->rear] = request;
    q->size++;
}

MemoryRequest dequeue(Queue *q) {
    if (isQueueEmpty(q)) {
        printf("Queue is empty, cannot dequeue\n");
        exit(1);
    }
    MemoryRequest request = q->requests[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->size--;
    return request;
}

void classifyThreads(ThreadInfo threads[], int num_threads) {
    for (int i = 0; i < num_threads; i++) {
        float mrpki = (float)threads[i].memory_requests / (threads[i].instructions / 1000.0);
        threads[i].is_memory_intensive = (mrpki > MEMORY_INTENSIVE_THRESHOLD);
        threads[i].priority = threads[i].is_memory_intensive ? 2 : 1;
    }
}

void adaptSchedulingPolicy(ThreadInfo threads[], int num_threads) {
    // Simple adaptation: Increase priority for memory-intensive threads
    for (int i = 0; i < num_threads; i++) {
        if (threads[i].is_memory_intensive) {
            threads[i].priority++;
        }
    }
}

MemoryRequest scheduleRequest(Queue *q, ThreadInfo threads[], int num_threads) {
    if (isQueueEmpty(q)) {
        printf("Queue is empty, cannot schedule\n");
        exit(1);
    }

    int highest_priority = 0;
    int selected_index = -1;

    for (int i = 0; i < q->size; i++) {
        int index = (q->front + i) % MAX_QUEUE_SIZE;
        int thread_id = q->requests[index].thread_id;
        if (threads[thread_id].priority > highest_priority) {
            highest_priority = threads[thread_id].priority;
            selected_index = index;
        }
    }

    if (selected_index == -1) {
        return dequeue(q);  // If no high priority request, just dequeue the oldest
    }

    // Remove the selected request from the queue
    MemoryRequest selected = q->requests[selected_index];
    for (int i = selected_index; i != q->rear; i = (i + 1) % MAX_QUEUE_SIZE) {
        q->requests[i] = q->requests[(i + 1) % MAX_QUEUE_SIZE];
    }
    q->rear = (q->rear - 1 + MAX_QUEUE_SIZE) % MAX_QUEUE_SIZE;
    q->size--;

    return selected;
}

int main() {
    Queue requestQueue;
    initQueue(&requestQueue);

    ThreadInfo threads[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++) {
        threads[i].memory_requests = 0;
        threads[i].instructions = 0;
        threads[i].is_memory_intensive = false;
        threads[i].priority = 1;
    }

    // Simulate memory requests and thread behavior
    for (int cycle = 0; cycle < 10000; cycle++) {
        // Simulate thread behavior
        for (int i = 0; i < MAX_THREADS; i++) {
            threads[i].instructions += rand() % 10 + 1;
            if (rand() % 10 < 3) {  // 30% chance of memory request
                MemoryRequest request;
                request.address = rand() % 1000;
                request.timestamp = time(NULL);
                request.thread_id = i;
                enqueue(&requestQueue, request);
                threads[i].memory_requests++;
            }
        }

        // Periodically adapt scheduling policy
        if (cycle % ADAPTATION_INTERVAL == 0) {
            classifyThreads(threads, MAX_THREADS);
            adaptSchedulingPolicy(threads, MAX_THREADS);
        }

        // Schedule and process a request
        if (!isQueueEmpty(&requestQueue)) {
            MemoryRequest scheduled = scheduleRequest(&requestQueue, threads, MAX_THREADS);
            printf("Cycle %d: Processed request from Thread %d, Address %d, Timestamp %ld\n", 
                   cycle, scheduled.thread_id, scheduled.address, scheduled.timestamp);
        }
    }

    // Print final thread statistics
    printf("\nFinal Thread Statistics:\n");
    for (int i = 0; i < MAX_THREADS; i++) {
        printf("Thread %d: Instructions %d, Memory Requests %d, Memory Intensive: %s, Priority: %d\n",
               i, threads[i].instructions, threads[i].memory_requests,
               threads[i].is_memory_intensive ? "Yes" : "No", threads[i].priority);
    }

    return 0;
}
