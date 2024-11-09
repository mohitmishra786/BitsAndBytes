#include <stdio.h>
#include <string.h>
#include "allocator.h"

void malloc_and_print(const char *label, size_t size, char fill) {
    void *ptr = m_malloc(size);
    if (ptr == NULL) {
        printf("%s: allocation failed\n", label);
        return;
    }
    memset(ptr, fill, size);
    printf("%s: %p\n", label, ptr);
}

int main() {
    m_init(20*1024*1024); // Initialize with 20MB

    printf("Initial heap state:\n");
    print_heap();

    malloc_and_print("A", 16, 'A');
    malloc_and_print("B", 20, 'B');
    malloc_and_print("C", 24, 'C');

    printf("\nHeap state after allocations:\n");
    print_heap();

    void *ptr = m_malloc(20);
    printf("Freeing %p\n", ptr);
    m_free(ptr);

    printf("\nHeap state after free:\n");
    print_heap();

    malloc_and_print("D", 8, 'D');

    printf("\nFinal heap state:\n");
    print_heap();

    printf("\nRunning garbage collection...\n");
    gc();

    printf("\nHeap state after garbage collection:\n");
    print_heap();

    return 0;
}

