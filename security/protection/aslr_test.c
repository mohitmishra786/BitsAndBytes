#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_addresses() {
    void* stack_var;
    void* heap_var = malloc(1);
    extern void* __libc_start_main;
    
    printf("Stack variable: %p\n", (void*)&stack_var);
    printf("Heap variable: %p\n", heap_var);
    printf("Libc function: %p\n", &__libc_start_main);
    printf("Main function: %p\n", (void*)&print_addresses);
    
    free(heap_var);
}

int main() {
    printf("Process ID: %d\n\n", getpid());
    
    for (int i = 0; i < 3; i++) {
        printf("Run %d:\n", i + 1);
        print_addresses();
        printf("\n");
        sleep(1);
    }
    return 0;
}