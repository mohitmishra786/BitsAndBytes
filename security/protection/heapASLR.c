#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void allocate_and_print(int count) {
    for (int i = 0; i < count; i++) {
        int *ptr = (int*)malloc(sizeof(int));
        printf("Heap allocation %d: %p\n", i + 1, (void*)ptr);
        free(ptr);
    }
}

int main() {
    printf("Running with PID: %d\n", getpid());
    allocate_and_print(5);
    return 0;
}
