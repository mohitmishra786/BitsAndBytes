#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TEST_SIZE (1024 * 1024 * 10) // 10MB of data

void test_write_performance(size_t buffer_size) {
    char* data = malloc(TEST_SIZE);
    if (!data) return;
    
    memset(data, 'A', TEST_SIZE);
    
    FILE* f = fopen("test.dat", "w");
    if (!f) {
        free(data);
        return;
    }
    
    clock_t start = clock();
    
    size_t remaining = TEST_SIZE;
    while (remaining > 0) {
        size_t to_write = remaining < buffer_size ? remaining : buffer_size;
        fwrite(data + (TEST_SIZE - remaining), 1, to_write, f);
        remaining -= to_write;
    }
    
    fclose(f);
    
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("Buffer size: %zu bytes, Time: %f seconds\n", 
           buffer_size, time_spent);
    
    free(data);
}

int main() {
    test_write_performance(1);        // Unbuffered
    test_write_performance(1024);     // 1KB buffer
    test_write_performance(4096);     // 4KB buffer
    test_write_performance(8192);     // 8KB buffer
    return 0;
}
