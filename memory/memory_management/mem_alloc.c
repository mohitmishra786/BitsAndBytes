#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 1024
#define MIN_ALLOCATION 16

typedef struct Block {
    size_t size;
    int is_free;
    struct Block* next;
} Block;

typedef enum {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
} AllocationStrategy;

static char memory_pool[MEMORY_SIZE];
static Block* head = NULL;

void* split_block(Block* block, size_t size);
void print_memory_state(void);

void init_memory() {
    head = (Block*)memory_pool;
    head->size = MEMORY_SIZE - sizeof(Block);
    head->is_free = 1;
    head->next = NULL;
    printf("Memory initialized: Total size = %zu bytes\n", MEMORY_SIZE);
}

void* allocate_memory(size_t size, AllocationStrategy strategy) {
    Block *current, *best_block = NULL;
    size_t min_size = MEMORY_SIZE + 1;
    size_t max_size = 0;
    void* result = NULL;
    
    size = (size + MIN_ALLOCATION - 1) & ~(MIN_ALLOCATION - 1);
    
    switch(strategy) {
        case FIRST_FIT:
            current = head;
            while(current) {
                if(current->is_free && current->size >= size) {
                    result = split_block(current, size);
                    printf("First Fit: Allocated %zu bytes\n", size);
                    return result;
                }
                current = current->next;
            }
            break;
            
        case BEST_FIT:
            current = head;
            while(current) {
                if(current->is_free && current->size >= size) {
                    if(current->size < min_size) {
                        min_size = current->size;
                        best_block = current;
                    }
                }
                current = current->next;
            }
            if(best_block) {
                result = split_block(best_block, size);
                printf("Best Fit: Allocated %zu bytes\n", size);
                return result;
            }
            break;
            
        case WORST_FIT:
            current = head;
            while(current) {
                if(current->is_free && current->size >= size) {
                    if(current->size > max_size) {
                        max_size = current->size;
                        best_block = current;
                    }
                }
                current = current->next;
            }
            if(best_block) {
                result = split_block(best_block, size);
                printf("Worst Fit: Allocated %zu bytes\n", size);
                return result;
            }
            break;
    }
    
    printf("Failed to allocate %zu bytes\n", size);
    return NULL;
}

void* split_block(Block* block, size_t size) {
    if(block->size > size + sizeof(Block) + MIN_ALLOCATION) {
        Block* new_block = (Block*)((char*)block + sizeof(Block) + size);
        new_block->size = block->size - size - sizeof(Block);
        new_block->is_free = 1;
        new_block->next = block->next;
        
        block->size = size;
        block->is_free = 0;
        block->next = new_block;
    } else {
        block->is_free = 0;
    }
    
    return (void*)((char*)block + sizeof(Block));
}

void free_memory(void* ptr) {
    if(!ptr) return;
    
    Block* block = (Block*)((char*)ptr - sizeof(Block));
    block->is_free = 1;
    
    if(block->next && block->next->is_free) {
        block->size += sizeof(Block) + block->next->size;
        block->next = block->next->next;
    }
    
    Block* prev = head;
    while(prev && prev->next != block) {
        prev = prev->next;
    }
    
    if(prev && prev->is_free) {
        prev->size += sizeof(Block) + block->size;
        prev->next = block->next;
    }
    
    printf("Freed memory block\n");
}

void print_memory_state() {
    Block* current = head;
    int block_count = 0;
    size_t total_free = 0;
    size_t total_used = 0;
    
    printf("\n=== Memory State ===\n");
    while(current) {
        printf("Block %d: Size = %zu, Status = %s\n",
               block_count++,
               current->size,
               current->is_free ? "FREE" : "USED");
        
        if(current->is_free) {
            total_free += current->size;
        } else {
            total_used += current->size;
        }
        current = current->next;
    }
    
    printf("\nTotal Free: %zu bytes\n", total_free);
    printf("Total Used: %zu bytes\n", total_used);
    printf("==================\n\n");
}

int main() {
    printf("Starting Memory Allocation Test\n\n");
    init_memory();
    print_memory_state();
    
    printf("Testing First Fit Strategy:\n");
    void* p1 = allocate_memory(100, FIRST_FIT);
    print_memory_state();
    
    void* p2 = allocate_memory(200, FIRST_FIT);
    print_memory_state();
    
    printf("\nFreeing first allocation...\n");
    free_memory(p1);
    print_memory_state();
    
    void* p3 = allocate_memory(150, FIRST_FIT);
    print_memory_state();
    
    printf("\nTesting Best Fit Strategy:\n");
    void* p4 = allocate_memory(50, BEST_FIT);
    print_memory_state();
    
    void* p5 = allocate_memory(75, BEST_FIT);
    print_memory_state();
    
    printf("\nFreeing fourth allocation...\n");
    free_memory(p4);
    print_memory_state();
    
    void* p6 = allocate_memory(25, BEST_FIT);
    print_memory_state();
    
    printf("\nCleaning up all allocations...\n");
    free_memory(p2);
    free_memory(p3);
    free_memory(p5);
    free_memory(p6);
    print_memory_state();
    
    printf("Memory Allocation Test Complete\n");
    return 0;
}