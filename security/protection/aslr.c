#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h> 

#define PAGE_SIZE 4096
#define MAX_PAGES 1000000

typedef struct {
    uintptr_t address; 
    int is_allocated;
} MemoryPage;

typedef struct {
    MemoryPage pages[MAX_PAGES];
    uintptr_t text_start;  
    uintptr_t data_start;
    uintptr_t heap_start;
    uintptr_t stack_start;
    uintptr_t mmap_start;
} MemoryLayout;

MemoryLayout g_memory_layout;

void init_memory_layout() {
    memset(&g_memory_layout, 0, sizeof(MemoryLayout));
    srand(time(NULL) ^ (getpid() << 16));
}

uintptr_t randomize_address(uintptr_t start, size_t range) {
    return (start + (rand() % range));
}

void* allocate_random_page() {
    int page_index;
    do {
        page_index = rand() % MAX_PAGES;
    } while (g_memory_layout.pages[page_index].is_allocated);

    g_memory_layout.pages[page_index].is_allocated = 1;
    g_memory_layout.pages[page_index].address = page_index * PAGE_SIZE; 
    return (void*)g_memory_layout.pages[page_index].address; 
}

void apply_aslr() {
    g_memory_layout.text_start = randomize_address(0x400000, 0x10000);
    g_memory_layout.data_start = randomize_address(0x600000, 0x10000);
    g_memory_layout.heap_start = randomize_address(0x800000, 0x100000);
    g_memory_layout.mmap_start = randomize_address(0xf7000000, 0x10000000);
    g_memory_layout.stack_start = randomize_address(0x7ffffffde000, 0x20000);
}

void* sim_malloc(size_t size) {
    uintptr_t addr = (uintptr_t)allocate_random_page();
    return (void*)(addr + (rand() % (PAGE_SIZE - size)));
}

void* sim_alloca(size_t size) {
    return (void*)(g_memory_layout.stack_start - (rand() % (0x20000 - size)));
}

void* load_shared_library(const char* name) {
    return (void*)randomize_address(g_memory_layout.mmap_start, 0x10000000);
}

void stack_function() {
    int stack_var;
    printf("Stack variable address: %p\n", (void*)&stack_var);
}

int main() {
    init_memory_layout();
    apply_aslr();

    printf("Process ID: %d\n", getpid());
    printf("Text segment start: %p\n", (void*)g_memory_layout.text_start);
    printf("Data segment start: %p\n", (void*)g_memory_layout.data_start);
    printf("Heap start: %p\n", (void*)g_memory_layout.heap_start);
    printf("Stack start: %p\n", (void*)g_memory_layout.stack_start);
    printf("mmap region start: %p\n", (void*)g_memory_layout.mmap_start);

    printf("\nStack Randomization:\n");
    for (int i = 0; i < 3; i++) {
        stack_function();
    }

    printf("\nHeap Randomization:\n");
    for (int i = 0; i < 3; i++) {
        void* heap_var = sim_malloc(sizeof(int));
        printf("Heap allocation %d: %p\n", i + 1, heap_var);
    }

    printf("\nShared Library Randomization:\n");
    for (int i = 0; i < 3; i++) {
        void* lib_addr = load_shared_library("example_lib.so");
        printf("Shared library %d loaded at: %p\n", i + 1, lib_addr);
    }

    return 0;
}
