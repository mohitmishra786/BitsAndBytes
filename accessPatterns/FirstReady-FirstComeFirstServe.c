#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_QUEUE_SIZE 100

typedef struct {
    int address;
    time_t timestamp;
    bool is_row_buffer_hit;
} MemoryRequest;

typedef struct {
    MemoryRequest requests[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
} Queue;

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

MemoryRequest frFcfsSchedule(Queue *hitQueue, Queue *missQueue) {
    if (!isQueueEmpty(hitQueue)) {
        return dequeue(hitQueue);
    } else if (!isQueueEmpty(missQueue)) {
        return dequeue(missQueue);
    } else {
        printf("Both queues are empty\n");
        exit(1);
    }
}

int main() {
    Queue hitQueue, missQueue;
    initQueue(&hitQueue);
    initQueue(&missQueue);

    // Simulate memory requests
    for (int i = 0; i < 10; i++) {
        MemoryRequest request;
        request.address = rand() % 1000;
        request.timestamp = time(NULL);
        request.is_row_buffer_hit = rand() % 2;

        if (request.is_row_buffer_hit) {
            enqueue(&hitQueue, request);
        } else {
            enqueue(&missQueue, request);
        }
    }

    // Schedule and process requests
    while (!isQueueEmpty(&hitQueue) || !isQueueEmpty(&missQueue)) {
        MemoryRequest scheduled = frFcfsSchedule(&hitQueue, &missQueue);
        printf("Processed request: Address %d, Timestamp %ld, Is Hit: %d\n", 
               scheduled.address, scheduled.timestamp, scheduled.is_row_buffer_hit);
    }

    return 0;
}
