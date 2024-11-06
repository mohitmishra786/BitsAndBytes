/**
 * complete_queue.c
 * A comprehensive implementation of a circular queue data structure
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <assert.h>

/* Constants */
#define QUEUE_EMPTY INT_MIN
#define DEFAULT_CAPACITY 8

/* Error Codes */
typedef enum {
    QUEUE_SUCCESS = 0,
    QUEUE_ERROR_FULL = -1,
    QUEUE_ERROR_EMPTY = -2,
    QUEUE_ERROR_MEMORY = -3,
    QUEUE_ERROR_INVALID = -4
} QueueError;

/* Queue Structure */
typedef struct {
    int* values;         // Dynamic array to store elements
    int head;           // Index of the front element
    int tail;           // Index of the next free position
    int num_entries;    // Current number of elements
    int size;           // Maximum capacity of the queue
    bool initialized;   // Flag to track initialization status
} Queue;

/* Function Prototypes */
QueueError queue_init(Queue* q, int size);
QueueError queue_destroy(Queue* q);
bool queue_empty(const Queue* q);
bool queue_full(const Queue* q);
QueueError queue_enqueue(Queue* q, int value);
QueueError queue_dequeue(Queue* q, int* result);
int queue_peek(const Queue* q);
int queue_size(const Queue* q);
QueueError queue_clear(Queue* q);
void queue_print(const Queue* q);
QueueError queue_resize(Queue* q, int new_size);

/* Implementation */

/**
 * Initializes a new queue with the specified size
 * @param q Pointer to queue structure
 * @param size Desired size of queue
 * @return QUEUE_SUCCESS on success, error code otherwise
 */
QueueError queue_init(Queue* q, int size) {
    if (q == NULL || size <= 0) {
        return QUEUE_ERROR_INVALID;
    }

    q->values = (int*)malloc(size * sizeof(int));
    if (q->values == NULL) {
        return QUEUE_ERROR_MEMORY;
    }

    q->size = size;
    q->head = 0;
    q->tail = 0;
    q->num_entries = 0;
    q->initialized = true;

    return QUEUE_SUCCESS;
}

/**
 * Destroys the queue and frees allocated memory
 * @param q Pointer to queue structure
 * @return QUEUE_SUCCESS on success, error code otherwise
 */
QueueError queue_destroy(Queue* q) {
    if (q == NULL || !q->initialized) {
        return QUEUE_ERROR_INVALID;
    }

    free(q->values);
    q->values = NULL;
    q->initialized = false;
    q->size = 0;
    q->head = 0;
    q->tail = 0;
    q->num_entries = 0;

    return QUEUE_SUCCESS;
}

/**
 * Checks if the queue is empty
 * @param q Pointer to queue structure
 * @return true if empty, false otherwise
 */
bool queue_empty(const Queue* q) {
    assert(q != NULL && q->initialized);
    return q->num_entries == 0;
}

/**
 * Checks if the queue is full
 * @param q Pointer to queue structure
 * @return true if full, false otherwise
 */
bool queue_full(const Queue* q) {
    assert(q != NULL && q->initialized);
    return q->num_entries == q->size;
}

/**
 * Adds an element to the back of the queue
 * @param q Pointer to queue structure
 * @param value Value to enqueue
 * @return QUEUE_SUCCESS on success, error code otherwise
 */
QueueError queue_enqueue(Queue* q, int value) {
    if (q == NULL || !q->initialized) {
        return QUEUE_ERROR_INVALID;
    }

    if (queue_full(q)) {
        return QUEUE_ERROR_FULL;
    }

    q->values[q->tail] = value;
    q->tail = (q->tail + 1) % q->size;
    q->num_entries++;

    return QUEUE_SUCCESS;
}

/**
 * Removes and returns the front element from the queue
 * @param q Pointer to queue structure
 * @param result Pointer to store the dequeued value
 * @return QUEUE_SUCCESS on success, error code otherwise
 */
QueueError queue_dequeue(Queue* q, int* result) {
    if (q == NULL || !q->initialized || result == NULL) {
        return QUEUE_ERROR_INVALID;
    }

    if (queue_empty(q)) {
        return QUEUE_ERROR_EMPTY;
    }

    *result = q->values[q->head];
    q->head = (q->head + 1) % q->size;
    q->num_entries--;

    return QUEUE_SUCCESS;
}

/**
 * Returns the front element without removing it
 * @param q Pointer to queue structure
 * @return Front element or QUEUE_EMPTY if queue is empty
 */
int queue_peek(const Queue* q) {
    if (q == NULL || !q->initialized || queue_empty(q)) {
        return QUEUE_EMPTY;
    }
    return q->values[q->head];
}

/**
 * Returns the current number of elements in the queue
 * @param q Pointer to queue structure
 * @return Number of elements in queue
 */
int queue_size(const Queue* q) {
    assert(q != NULL && q->initialized);
    return q->num_entries;
}

/**
 * Removes all elements from the queue
 * @param q Pointer to queue structure
 * @return QUEUE_SUCCESS on success, error code otherwise
 */
QueueError queue_clear(Queue* q) {
    if (q == NULL || !q->initialized) {
        return QUEUE_ERROR_INVALID;
    }

    q->head = 0;
    q->tail = 0;
    q->num_entries = 0;

    return QUEUE_SUCCESS;
}

/**
 * Resizes the queue to a new capacity
 * @param q Pointer to queue structure
 * @param new_size New desired size
 * @return QUEUE_SUCCESS on success, error code otherwise
 */
QueueError queue_resize(Queue* q, int new_size) {
    if (q == NULL || !q->initialized || new_size <= 0) {
        return QUEUE_ERROR_INVALID;
    }

    if (new_size < q->num_entries) {
        return QUEUE_ERROR_INVALID;
    }

    int* new_values = (int*)malloc(new_size * sizeof(int));
    if (new_values == NULL) {
        return QUEUE_ERROR_MEMORY;
    }

    // Copy existing elements to new array
    int count = 0;
    while (!queue_empty(q)) {
        int value;
        queue_dequeue(q, &value);
        new_values[count++] = value;
    }

    // Free old array and update queue
    free(q->values);
    q->values = new_values;
    q->size = new_size;
    q->head = 0;
    q->tail = count;
    q->num_entries = count;

    return QUEUE_SUCCESS;
}

/**
 * Prints the contents of the queue for debugging
 * @param q Pointer to queue structure
 */
void queue_print(const Queue* q) {
    if (q == NULL || !q->initialized) {
        printf("Queue is invalid or uninitialized\n");
        return;
    }

    if (queue_empty(q)) {
        printf("Queue is empty\n");
        return;
    }

    printf("Queue contents (head=%d, tail=%d, size=%d):\n", q->head, q->tail, q->num_entries);
    int current = q->head;
    int count = 0;
    while (count < q->num_entries) {
        printf("[%d]=%d ", current, q->values[current]);
        current = (current + 1) % q->size;
        count++;
    }
    printf("\n");
}

void print_operation_result(const char* operation, QueueError result) {
    printf("Operation: %-20s | Result: ", operation);
    switch(result) {
        case QUEUE_SUCCESS:
            printf("\033[0;32mSUCCESS\033[0m\n");  // Green
            break;
        case QUEUE_ERROR_FULL:
            printf("\033[0;31mQUEUE FULL\033[0m\n");  // Red
            break;
        case QUEUE_ERROR_EMPTY:
            printf("\033[0;31mQUEUE EMPTY\033[0m\n");  // Red
            break;
        case QUEUE_ERROR_MEMORY:
            printf("\033[0;31mMEMORY ERROR\033[0m\n");  // Red
            break;
        case QUEUE_ERROR_INVALID:
            printf("\033[0;31mINVALID OPERATION\033[0m\n");  // Red
            break;
        default:
            printf("\033[0;31mUNKNOWN ERROR\033[0m\n");  // Red
    }
}

/**
 * Comprehensive test suite
 */
void run_comprehensive_tests(void) {
    Queue q;
    int result;
    QueueError err;
    
    printf("\n=== Starting Comprehensive Queue Tests ===\n\n");

    // Test 1: Initialization
    printf("Test Set 1: Basic Initialization\n");
    printf("--------------------------------------\n");
    err = queue_init(&q, 5);
    print_operation_result("Initialize Queue", err);
    printf("Queue Status: Size=%d, Entries=%d\n", q.size, q.num_entries);
    queue_print(&q);
    printf("\n");

    // Test 2: Basic Operations
    printf("Test Set 2: Basic Enqueue Operations\n");
    printf("--------------------------------------\n");
    for (int i = 1; i <= 6; i++) {
        err = queue_enqueue(&q, i * 10);
        printf("Enqueuing %d: ", i * 10);
        print_operation_result("Enqueue", err);
        queue_print(&q);
    }
    printf("\n");

    // Test 3: Dequeue Operations
    printf("Test Set 3: Basic Dequeue Operations\n");
    printf("--------------------------------------\n");
    for (int i = 0; i < 3; i++) {
        err = queue_dequeue(&q, &result);
        if (err == QUEUE_SUCCESS) {
            printf("Dequeued value: %d\n", result);
        }
        print_operation_result("Dequeue", err);
        queue_print(&q);
    }
    printf("\n");

    // Test 4: Wraparound Behavior
    printf("Test Set 4: Testing Wraparound\n");
    printf("--------------------------------------\n");
    for (int i = 1; i <= 3; i++) {
        err = queue_enqueue(&q, i * 100);
        printf("Enqueuing %d: ", i * 100);
        print_operation_result("Enqueue", err);
        queue_print(&q);
    }
    printf("\n");

    // Test 5: Clear Operation
    printf("Test Set 5: Clear Operation\n");
    printf("--------------------------------------\n");
    err = queue_clear(&q);
    print_operation_result("Clear Queue", err);
    queue_print(&q);
    printf("\n");

    // Test 6: Operations after Clear
    printf("Test Set 6: Post-Clear Operations\n");
    printf("--------------------------------------\n");
    err = queue_enqueue(&q, 42);
    print_operation_result("Enqueue after clear", err);
    queue_print(&q);
    printf("\n");

    // Test 7: Resize Operation
    printf("Test Set 7: Resize Operation\n");
    printf("--------------------------------------\n");
    err = queue_resize(&q, 10);
    print_operation_result("Resize to 10", err);
    printf("New queue size: %d\n", q.size);
    queue_print(&q);
    printf("\n");

    // Test 8: Fill Resized Queue
    printf("Test Set 8: Fill Resized Queue\n");
    printf("--------------------------------------\n");
    for (int i = 1; i <= 8; i++) {
        err = queue_enqueue(&q, i * 5);
        printf("Enqueuing %d: ", i * 5);
        print_operation_result("Enqueue", err);
    }
    queue_print(&q);
    printf("\n");

    // Test 9: Peek Operation
    printf("Test Set 9: Peek Operation\n");
    printf("--------------------------------------\n");
    int peek_val = queue_peek(&q);
    printf("Peek value: %d\n", peek_val);
    queue_print(&q);
    printf("\n");

    // Test 10: Error Conditions
    printf("Test Set 10: Error Conditions\n");
    printf("--------------------------------------\n");
    err = queue_enqueue(&q, 999);  // Should fail, queue is full
    print_operation_result("Enqueue to full queue", err);
    
    queue_clear(&q);
    err = queue_dequeue(&q, &result);  // Should fail, queue is empty
    print_operation_result("Dequeue from empty", err);
    printf("\n");

    // Cleanup
    printf("Test Set 11: Cleanup\n");
    printf("--------------------------------------\n");
    err = queue_destroy(&q);
    print_operation_result("Destroy Queue", err);

    printf("\n=== Queue Tests Completed ===\n\n");
}

int main(void) {
    run_comprehensive_tests();
    return 0;
}
