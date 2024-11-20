#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

// Structure to measure timing
typedef struct {
    struct timeval start;
    struct timeval end;
} Timer;

void start_timer(Timer* timer) {
    gettimeofday(&timer->start, NULL);
}

double end_timer(Timer* timer) {
    gettimeofday(&timer->end, NULL);
    return (timer->end.tv_sec - timer->start.tv_sec) * 1000000.0 +
           (timer->end.tv_usec - timer->start.tv_usec);
}

// Structure to simulate different sized allocations
typedef struct {
    char* data;
    size_t size;
} DataBlock;

// Test allocation patterns
void test_allocation_pattern(size_t block_size, int num_allocations, const char* pattern_name) {
    Timer timer;
    DataBlock* blocks = malloc(sizeof(DataBlock) * num_allocations);
    double total_alloc_time = 0;
    double total_free_time = 0;
    
    printf("\nTesting %s (Block size: %zu bytes, Count: %d)\n", 
           pattern_name, block_size, num_allocations);
    
    // Allocation phase
    start_timer(&timer);
    for(int i = 0; i < num_allocations; i++) {
        blocks[i].size = block_size;
        blocks[i].data = malloc(block_size);
        // Touch the memory to ensure it's actually allocated
        memset(blocks[i].data, 'A', block_size);
    }
    total_alloc_time = end_timer(&timer);
    
    // Free phase
    start_timer(&timer);
    for(int i = 0; i < num_allocations; i++) {
        free(blocks[i].data);
    }
    total_free_time = end_timer(&timer);
    
    // Calculate statistics
    double avg_alloc_time = total_alloc_time / num_allocations;
    double avg_free_time = total_free_time / num_allocations;
    
    printf("Average allocation time: %.2f microseconds\n", avg_alloc_time);
    printf("Average deallocation time: %.2f microseconds\n", avg_free_time);
    printf("Total overhead per operation: %.2f microseconds\n", 
           avg_alloc_time + avg_free_time);
    
    free(blocks);
}

// Test fragmentation impact
void test_fragmentation_impact(int num_iterations) {
    Timer timer;
    const int NUM_BLOCKS = 1000;
    void* blocks[NUM_BLOCKS];
    
    printf("\nTesting Fragmentation Impact\n");
    
    start_timer(&timer);
    for(int iter = 0; iter < num_iterations; iter++) {
        // Allocate blocks of varying sizes
        for(int i = 0; i < NUM_BLOCKS; i++) {
            size_t size = (rand() % 1000) + 1;  // 1 to 1000 bytes
            blocks[i] = malloc(size);
        }
        
        // Free every other block to create fragmentation
        for(int i = 0; i < NUM_BLOCKS; i += 2) {
            free(blocks[i]);
        }
        
        // Try to allocate a large block
        void* large_block = malloc(10000);
        free(large_block);
        
        // Clean up remaining blocks
        for(int i = 1; i < NUM_BLOCKS; i += 2) {
            free(blocks[i]);
        }
    }
    double total_time = end_timer(&timer);
    
    printf("Average time per fragmentation cycle: %.2f microseconds\n", 
           total_time / num_iterations);
}

// Test system call overhead
void test_syscall_overhead() {
    Timer timer;
    const int NUM_TESTS = 1000;
    const size_t LARGE_SIZE = 1024 * 1024;  // 1MB
    
    printf("\nTesting System Call Overhead\n");
    
    // Test small allocations (likely no system call)
    start_timer(&timer);
    for(int i = 0; i < NUM_TESTS; i++) {
        void* ptr = malloc(64);  // Small allocation
        free(ptr);
    }
    double small_time = end_timer(&timer);
    
    // Test large allocations (likely triggering system calls)
    start_timer(&timer);
    for(int i = 0; i < NUM_TESTS; i++) {
        void* ptr = malloc(LARGE_SIZE);  // Large allocation
        free(ptr);
    }
    double large_time = end_timer(&timer);
    
    printf("Average time for small allocation/free: %.2f microseconds\n", 
           small_time / NUM_TESTS);
    printf("Average time for large allocation/free: %.2f microseconds\n", 
           large_time / NUM_TESTS);
    printf("System call overhead ratio: %.2fx\n", 
           large_time / small_time);
}

int main() {
    srand(time(NULL));
    
    // Test different allocation patterns
    test_allocation_pattern(64, 10000, "Small Fixed Allocations");
    test_allocation_pattern(1024 * 1024, 100, "Large Fixed Allocations");
    
    // Test fragmentation impact
    test_fragmentation_impact(100);
    
    // Test system call overhead
    test_syscall_overhead();
    
    return 0;
}