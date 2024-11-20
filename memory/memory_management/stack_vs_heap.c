#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char data[1024];
    int size;
} LargeObject;

// Stack-based implementation
void process_on_stack() {
    LargeObject obj;
    obj.size = 1024;
    memset(obj.data, 'A', obj.size);
}

// Heap-based implementation
void process_on_heap() {
    LargeObject* obj = (LargeObject*)malloc(sizeof(LargeObject));
    if(!obj) {
        printf("Allocation failed!\n");
        return;
    }
    obj->size = 1024;
    memset(obj->data, 'A', obj->size);
    free(obj);
}

int main() {
    const int NUM_ITERATIONS = 100000;
    
    // Time stack operations
    clock_t start = clock();
    for(int i = 0; i < NUM_ITERATIONS; i++) {
        process_on_stack();
    }
    clock_t stack_time = clock() - start;
    
    // Time heap operations
    start = clock();
    for(int i = 0; i < NUM_ITERATIONS; i++) {
        process_on_heap();
    }
    clock_t heap_time = clock() - start;
    
    printf("Stack time: %f seconds\n", 
           (double)stack_time / CLOCKS_PER_SEC);
    printf("Heap time: %f seconds\n", 
           (double)heap_time / CLOCKS_PER_SEC);
    
    return 0;
}