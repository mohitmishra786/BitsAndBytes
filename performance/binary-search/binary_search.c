// binary_search.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <x86intrin.h> // For rdtsc
#include <string.h>

// Assembly implementation
extern int binary_search_asm(int* arr, int size, int target);

// C implementation
int binary_search_c(int* arr, int size, int target) {
    int left = 0;
    int right = size - 1;
    
    while (left <= right) {
        int mid = left + ((right - left) >> 1); // Avoid overflow
        
        if (arr[mid] == target)
            return mid;
        
        if (arr[mid] < target)
            left = mid + 1;
        else
            right = mid - 1;
    }
    
    return -1;
}

// Utility function to generate sorted test data
void generate_test_data(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = i * 2; // Creates sorted array with gaps
    }
}

// Performance testing function
void run_performance_test(int size) {
    int* arr = (int*)aligned_alloc(64, size * sizeof(int)); // 64-byte aligned for cache optimization
    generate_test_data(arr, size);
    
    // Warm up the cache
    for (int i = 0; i < 1000; i++) {
        binary_search_c(arr, size, -1);
        binary_search_asm(arr, size, -1);
    }
    
    // Number of iterations for averaging
    const int iterations = 100000;
    
    // Test C implementation
    unsigned long long start_c = __rdtsc();
    clock_t time_start_c = clock();
    
    for (int i = 0; i < iterations; i++) {
        // Search for existing and non-existing values
        binary_search_c(arr, size, (i % size) * 2);
        binary_search_c(arr, size, (i % size) * 2 + 1);
    }
    
    clock_t time_end_c = clock();
    unsigned long long end_c = __rdtsc();
    
    // Test Assembly implementation
    unsigned long long start_asm = __rdtsc();
    clock_t time_start_asm = clock();
    
    for (int i = 0; i < iterations; i++) {
        binary_search_asm(arr, size, (i % size) * 2);
        binary_search_asm(arr, size, (i % size) * 2 + 1);
    }
    
    clock_t time_end_asm = clock();
    unsigned long long end_asm = __rdtsc();
    
    // Calculate and print results
    double time_c = ((double)(time_end_c - time_start_c)) / CLOCKS_PER_SEC;
    double time_asm = ((double)(time_end_asm - time_start_asm)) / CLOCKS_PER_SEC;
    
    printf("\nArray size: %d\n", size);
    printf("Iterations: %d\n\n", iterations);
    
    printf("C Implementation:\n");
    printf("Time: %.6f seconds\n", time_c);
    printf("Cycles: %llu\n", (end_c - start_c) / (iterations * 2));
    printf("Cycles per element: %.2f\n\n", (double)(end_c - start_c) / (iterations * 2 * size));
    
    printf("Assembly Implementation:\n");
    printf("Time: %.6f seconds\n", time_asm);
    printf("Cycles: %llu\n", (end_asm - start_asm) / (iterations * 2));
    printf("Cycles per element: %.2f\n\n", (double)(end_asm - start_asm) / (iterations * 2 * size));
    
    free(arr);
}

int main() {
    // Test with different array sizes
    int sizes[] = {100, 1000, 10000, 100000};
    
    for (int i = 0; i < sizeof(sizes)/sizeof(sizes[0]); i++) {
        run_performance_test(sizes[i]);
    }
    
    return 0;
}