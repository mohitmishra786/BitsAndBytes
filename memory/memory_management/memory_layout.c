#include <stdio.h>
#include <stdlib.h>

// Data segment
int global_initialized = 42;

// BSS segment
int global_uninitialized;

// Function to print address information
void print_address_info(void* ptr, const char* description) {
    printf("%s at address: %p\n", description, ptr);
}

int main() {
    // Stack variables
    int stack_var = 10;
    
    // Heap allocation
    int* heap_var = (int*)malloc(sizeof(int));
    *heap_var = 20;
    
    // Print addresses of variables in different segments
    print_address_info(&global_initialized, "Global initialized variable");
    print_address_info(&global_uninitialized, "Global uninitialized variable");
    print_address_info(&stack_var, "Stack variable");
    print_address_info(heap_var, "Heap variable");
    print_address_info((void*)main, "Program code (text)");
    
    free(heap_var);
    return 0;
}