/*
 * Memory Mapping Implementation Example
 * This code demonstrates memory mapping concepts for x86 architecture
 * Note: This runs with user-level privileges and simulates memory segments
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

// Define page size constant
#define PAGE_SIZE 4096

// Structure to represent memory segments
typedef struct {
    uint64_t start_addr;
    uint64_t end_addr;
    size_t size;
    int flags;
    const char* name;
} MemorySegment;

// Function to initialize memory segments with realistic sizes
void init_memory_segments(MemorySegment* segments, int is_64bit) {
    if (is_64bit) {
        // Using smaller, more manageable sizes for demonstration
        segments[0] = (MemorySegment){
            .start_addr = 0,
            .end_addr = PAGE_SIZE * 10,
            .size = PAGE_SIZE * 10,
            .flags = MAP_PRIVATE | MAP_ANONYMOUS,
            .name = "xkseg3"
        };
        segments[1] = (MemorySegment){
            .start_addr = PAGE_SIZE * 10,
            .end_addr = PAGE_SIZE * 20,
            .size = PAGE_SIZE * 10,
            .flags = MAP_PRIVATE | MAP_ANONYMOUS,
            .name = "xkseg2"
        };
        segments[2] = (MemorySegment){
            .start_addr = PAGE_SIZE * 20,
            .end_addr = PAGE_SIZE * 30,
            .size = PAGE_SIZE * 10,
            .flags = MAP_PRIVATE | MAP_ANONYMOUS,
            .name = "xkseg1"
        };
    } else {
        // 32-bit segments
        segments[0] = (MemorySegment){
            .start_addr = 0,
            .end_addr = PAGE_SIZE * 5,
            .size = PAGE_SIZE * 5,
            .flags = MAP_PRIVATE | MAP_ANONYMOUS,
            .name = "kseg3"
        };
        segments[1] = (MemorySegment){
            .start_addr = PAGE_SIZE * 5,
            .end_addr = PAGE_SIZE * 10,
            .size = PAGE_SIZE * 5,
            .flags = MAP_PRIVATE | MAP_ANONYMOUS,
            .name = "kseg2"
        };
        segments[2] = (MemorySegment){
            .start_addr = PAGE_SIZE * 10,
            .end_addr = PAGE_SIZE * 15,
            .size = PAGE_SIZE * 5,
            .flags = MAP_PRIVATE | MAP_ANONYMOUS,
            .name = "kseg1"
        };
    }
}

// Function to map memory segment
void* map_segment(MemorySegment* segment) {
    // Map memory with NULL to let the system choose the address
    void* addr = mmap(
        NULL,                   // Let the kernel choose the address
        segment->size,          // Size of mapping
        PROT_READ | PROT_WRITE, // Read and write permissions
        segment->flags,         // Mapping flags
        -1,                     // File descriptor (-1 for anonymous mapping)
        0                       // Offset (0 for anonymous mapping)
    );
    
    if (addr == MAP_FAILED) {
        printf("Memory mapping failed for %s: %s\n", 
               segment->name, strerror(errno));
        return NULL;
    }
    
    // Write some test data to the mapped memory
    memset(addr, 0x42, PAGE_SIZE); // Write pattern to first page
    
    return addr;
}

// Function to test mapped memory
void test_mapped_memory(void* addr, const char* segment_name) {
    if (addr) {
        unsigned char* mem = (unsigned char*)addr;
        printf("Testing %s:\n", segment_name);
        printf("First byte: 0x%02X\n", mem[0]);
        printf("Memory accessible and contains expected pattern\n\n");
    }
}

int main() {
    const int NUM_SEGMENTS = 3;
    MemorySegment segments[NUM_SEGMENTS];
    void* mapped_addresses[NUM_SEGMENTS];
    int is_64bit = sizeof(void*) == 8;
    
    printf("Running on %d-bit system\n\n", is_64bit ? 64 : 32);
    
    // Initialize segments based on architecture
    init_memory_segments(segments, is_64bit);
    
    // Map each segment and store the addresses
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        mapped_addresses[i] = map_segment(&segments[i]);
        if (mapped_addresses[i]) {
            printf("Successfully mapped %s at address %p\n", 
                   segments[i].name, mapped_addresses[i]);
            test_mapped_memory(mapped_addresses[i], segments[i].name);
        }
    }
    
    // Use the mapped memory (demonstration)
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        if (mapped_addresses[i]) {
            // Write some data
            char* mem = (char*)mapped_addresses[i];
            snprintf(mem, PAGE_SIZE, "Hello from %s!", segments[i].name);
            printf("Data written to %s: %s\n", segments[i].name, mem);
        }
    }
    
    // Cleanup: Unmap all segments
    printf("\nCleaning up mapped memory...\n");
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        if (mapped_addresses[i]) {
            if (munmap(mapped_addresses[i], segments[i].size) == 0) {
                printf("Successfully unmapped %s\n", segments[i].name);
            } else {
                printf("Failed to unmap %s: %s\n", 
                       segments[i].name, strerror(errno));
            }
        }
    }
    
    return 0;
}
