#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALLOCATIONS 1000

typedef struct {
    void* ptr;
    size_t size;
    const char* file;
    int line;
} AllocationInfo;

AllocationInfo allocations[MAX_ALLOCATIONS];
int allocation_count = 0;

void* debug_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (ptr) {
        allocations[allocation_count].ptr = ptr;
        allocations[allocation_count].size = size;
        allocations[allocation_count].file = file;
        allocations[allocation_count].line = line;
        allocation_count++;
    }
    printf("Allocated %zu bytes at %p (%s:%d)\n", size, ptr, file, line);
    return ptr;
}

void debug_free(void* ptr, const char* file, int line) {
    for (int i = 0; i < allocation_count; i++) {
        if (allocations[i].ptr == ptr) {
            printf("Freed %zu bytes at %p (%s:%d)\n", allocations[i].size, ptr, file, line);
            free(ptr);
            allocations[i] = allocations[allocation_count - 1];
            allocation_count--;
            return;
        }
    }
    printf("Attempt to free unallocated pointer %p (%s:%d)\n", ptr, file, line);
}

#define malloc(size) debug_malloc(size, __FILE__, __LINE__)
#define free(ptr) debug_free(ptr, __FILE__, __LINE__)

int main() {
    int* numbers = (int*)malloc(5 * sizeof(int));
    free(numbers);
    free(numbers); // Double free
    int* unallocated = (int*)malloc(10 * sizeof(int));
    return 0;
}
