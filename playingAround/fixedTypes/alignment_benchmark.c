#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define ITERATIONS 100000000
#define NUM_RUNS 10  // Number of benchmark runs

// Poorly aligned structure
typedef struct {
    char a;      // 1 byte
    int32_t b;   // 4 bytes
    char c;      // 1 byte
} BadStruct;     // Actual size: 12 bytes due to padding

// Well aligned structure
typedef struct {
    int32_t b;   // 4 bytes
    char a;      // 1 byte
    char c;      // 1 byte
    // 2 bytes padding
} GoodStruct;    // Actual size: 8 bytes

// Function to print memory layout of a structure
void print_memory_layout(void* ptr, size_t size) {
    unsigned char* bytes = (unsigned char*)ptr;
    printf("Memory layout (hex): ");
    for (size_t i = 0; i < size; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");
}

double benchmark_aligned_access() {
    GoodStruct* data = (GoodStruct*)malloc(sizeof(GoodStruct));
    if (data == NULL) {
        printf("Memory allocation failed for GoodStruct\n");
        return 0.0;
    }
    
    // Initialize structure
    data->b = 0;
    data->a = 1;
    data->c = 2;
    
    // Ensure CPU cache is in a known state
    volatile int32_t temp = 0;
    
    clock_t start = clock();
    
    for (int i = 0; i < ITERATIONS; i++) {
        data->b += 1;  // Aligned access
        temp += data->b; // Prevent optimization
    }
    
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    
    free(data);
    return time_spent;
}

double benchmark_unaligned_access() {
    BadStruct* data = (BadStruct*)malloc(sizeof(BadStruct));
    if (data == NULL) {
        printf("Memory allocation failed for BadStruct\n");
        return 0.0;
    }
    
    // Initialize structure
    data->a = 1;
    data->b = 0;
    data->c = 2;
    
    // Ensure CPU cache is in a known state
    volatile int32_t temp = 0;
    
    clock_t start = clock();
    
    for (int i = 0; i < ITERATIONS; i++) {
        data->b += 1;  // Unaligned access
        temp += data->b; // Prevent optimization
    }
    
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    
    free(data);
    return time_spent;
}

void print_statistics(double times[], int count, const char* label) {
    double sum = 0.0;
    double min = times[0];
    double max = times[0];
    
    for (int i = 0; i < count; i++) {
        sum += times[i];
        if (times[i] < min) min = times[i];
        if (times[i] > max) max = times[i];
    }
    
    double avg = sum / count;
    
    // Calculate standard deviation
    double variance = 0.0;
    for (int i = 0; i < count; i++) {
        variance += (times[i] - avg) * (times[i] - avg);
    }
    double std_dev = sqrt(variance / count);
    
    printf("\n%s Statistics:\n", label);
    printf("Average time: %f seconds\n", avg);
    printf("Min time: %f seconds\n", min);
    printf("Max time: %f seconds\n", max);
    printf("Standard deviation: %f seconds\n", std_dev);
}

int main() {
    printf("Size of BadStruct: %zu bytes\n", sizeof(BadStruct));
    printf("Size of GoodStruct: %zu bytes\n", sizeof(GoodStruct));
    
    double aligned_times[NUM_RUNS];
    double unaligned_times[NUM_RUNS];
    
    printf("\nRunning benchmarks (%d iterations each, %d runs)...\n", 
           ITERATIONS, NUM_RUNS);
    
    for (int i = 0; i < NUM_RUNS; i++) {
        printf("Run %d/%d...\n", i + 1, NUM_RUNS);
        aligned_times[i] = benchmark_aligned_access();
        unaligned_times[i] = benchmark_unaligned_access();
    }
    
    print_statistics(aligned_times, NUM_RUNS, "Aligned Access");
    print_statistics(unaligned_times, NUM_RUNS, "Unaligned Access");
    
    return 0;
}
