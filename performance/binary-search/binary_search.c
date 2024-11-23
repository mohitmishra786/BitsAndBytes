#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <x86intrin.h>
#include <string.h>

extern int binary_search_asm(int* arr, int size, int target);

__attribute__((always_inline)) inline int binary_search_c(int* __restrict__ arr, int size, int target) {
    int left = 0;
    int right = size - 1;
    
    while (left <= right) {
        int mid = (left + right) >> 1;
        int mid_val = arr[mid];
        
        if (mid_val == target)
            return mid;
            
        if (mid_val < target)
            left = mid + 1;
        else
            right = mid - 1;
    }
    
    return -1;
}

void generate_test_data(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = i * 2;
    }
}

void run_performance_test(int size) {
    // aligning memory to cache line boundary
    int* arr = (int*)aligned_alloc(64, ((size * sizeof(int) + 63) / 64) * 64);
    if (!arr) {
        printf("Memory allocation failed!\n");
        return;
    }
    
    generate_test_data(arr, size);
    
    // Warm up phase - reduced iterations
    printf("Warming up...\n");
    for (int i = 0; i < 1000; i++) {
        binary_search_c(arr, size, (i % size) * 2);
        binary_search_asm(arr, size, (i % size) * 2);
    }
    
    const int iterations = 100000;
    const int print_interval = iterations / 10;
    
    printf("Starting C implementation test...\n");
    
    // testing C implementation
    unsigned long long start_c = __rdtsc();
    clock_t time_start_c = clock();
    
    volatile int result_c = 0;
    for (int i = 0; i < iterations; i++) {
        if (i % print_interval == 0) {
            printf("\rProgress: %d%%", (i * 100) / iterations);
            fflush(stdout);
        }
        
        result_c ^= binary_search_c(arr, size, (i % size) * 2);
        result_c ^= binary_search_c(arr, size, (i % size) * 2 + 1);
    }
    
    clock_t time_end_c = clock();
    unsigned long long end_c = __rdtsc();
    printf("\rProgress: 100%%\n");
    
    printf("Starting Assembly implementation test...\n");
    
    // let's do for asm
    unsigned long long start_asm = __rdtsc();
    clock_t time_start_asm = clock();
    
    volatile int result_asm = 0;
    for (int i = 0; i < iterations; i++) {
        if (i % print_interval == 0) {
            printf("\rProgress: %d%%", (i * 100) / iterations);
            fflush(stdout);
        }
        
        result_asm ^= binary_search_asm(arr, size, (i % size) * 2);
        result_asm ^= binary_search_asm(arr, size, (i % size) * 2 + 1);
    }
    
    clock_t time_end_asm = clock();
    unsigned long long end_asm = __rdtsc();
    printf("\rProgress: 100%%\n");
    
    // Calculate and print results
    double time_c = ((double)(time_end_c - time_start_c)) / CLOCKS_PER_SEC;
    double time_asm = ((double)(time_end_asm - time_start_asm)) / CLOCKS_PER_SEC;
    
    printf("\nArray size: %d\n", size);
    printf("Iterations: %d\n\n", iterations);
    
    printf("C Implementation:\n");
    printf("Time: %.6f seconds\n", time_c);
    printf("Average cycles per search: %.2f\n", (double)(end_c - start_c) / (iterations * 2));
    printf("Searches per second: %.2f\n\n", (iterations * 2) / time_c);
    
    printf("Assembly Implementation:\n");
    printf("Time: %.6f seconds\n", time_asm);
    printf("Average cycles per search: %.2f\n", (double)(end_asm - start_asm) / (iterations * 2));
    printf("Searches per second: %.2f\n\n", (iterations * 2) / time_asm);
    
    free(arr);
}

int main() {
    printf("Binary Search Performance Test\n");
    printf("==============================\n\n");
    
    // Test with different array sizes
    int sizes[] = {100, 1000, 10000, 100000};
    
    for (int i = 0; i < sizeof(sizes)/sizeof(sizes[0]); i++) {
        printf("Testing array size: %d\n", sizes[i]);
        printf("------------------------\n");
        run_performance_test(sizes[i]);
        printf("\n");
    }
    
    return 0;
}