#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_QUEUE_SIZE 100
#define MAX_BANKS 8
#define MAX_BATCH_SIZE 20

typedef struct {
    int address;
    time_t timestamp;
    int bank;
    bool is_row_buffer_hit;
    bool is_marked;
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

void formBatch(Queue *q, MemoryRequest batch[], int *batchSize) {
    *batchSize = 0;
    while (*batchSize < MAX_BATCH_SIZE && !isQueueEmpty(q)) {
        MemoryRequest request = dequeue(q);
        request.is_marked = true;
        batch[*batchSize] = request;
        (*batchSize)++;
    }
}

void rankBanks(MemoryRequest batch[], int batchSize, int bankRanking[]) {
    int bankCount[MAX_BANKS] = {0};
    for (int i = 0; i < batchSize; i++) {
        bankCount[batch[i].bank]++;
    }

    for (int i = 0; i < MAX_BANKS; i++) {
        bankRanking[i] = i;
    }

    // Simple bubble sort for ranking
    for (int i = 0; i < MAX_BANKS - 1; i++) {
        for (int j = 0; j < MAX_BANKS - i - 1; j++) {
            if (bankCount[bankRanking[j]] < bankCount[bankRanking[j + 1]]) {
                int temp = bankRanking[j];
                bankRanking[j] = bankRanking[j + 1];
                bankRanking[j + 1] = temp;
            }
        }
    }
}

MemoryRequest scheduleRequest(MemoryRequest batch[], int batchSize, int bankRanking[]) {
    for (int i = 0; i < MAX_BANKS; i++) {
        int bank = bankRanking[i];
        MemoryRequest *oldestHit = NULL;
        MemoryRequest *oldest = NULL;

        for (int j = 0; j < batchSize; j++) {
            if (batch[j].bank == bank && batch[j].is_marked) {
                if (batch[j].is_row_buffer_hit) {
                    if (oldestHit == NULL || batch[j].timestamp < oldestHit->timestamp) {
                        oldestHit = &batch[j];
                    }
                }
                if (oldest == NULL || batch[j].timestamp < oldest->timestamp) {
                    oldest = &batch[j];
                }
            }
        }

        if (oldestHit != NULL) {
            oldestHit->is_marked = false;
            return *oldestHit;
        } else if (oldest != NULL) {
            oldest->is_marked = false;
            return *oldest;
        }
    }

    printf("No request found to schedule\n");
    exit(1);
}

int main() {
    Queue requestQueue;
    initQueue(&requestQueue);

    // Simulate memory requests
    for (int i = 0; i < 50; i++) {
        MemoryRequest request;
        request.address = rand() % 1000;
        request.timestamp = time(NULL);
        request.bank = rand() % MAX_BANKS;
        request.is_row_buffer_hit = rand() % 2;
        request.is_marked = false;

        enqueue(&requestQueue, request);
    }

    while (!isQueueEmpty(&requestQueue)) {
        MemoryRequest batch[MAX_BATCH_SIZE];
        int batchSize;
        formBatch(&requestQueue, batch, &batchSize);

        int bankRanking[MAX_BANKS];
        rankBanks(batch, batchSize, bankRanking);

        for (int i = 0; i < batchSize; i++) {
            MemoryRequest scheduled = scheduleRequest(batch, batchSize, bankRanking);
            printf("Processed request: Address %d, Bank %d, Timestamp %ld, Is Hit: %d\n", 
                   scheduled.address, scheduled.bank, scheduled.timestamp, scheduled.is_row_buffer_hit);
        }
    }

    return 0;
}
