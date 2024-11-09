#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define L1_CACHE_SIZE 64
#define L2_CACHE_SIZE 256
#define MAIN_MEMORY_SIZE 8192
#define BLOCK_SIZE 8

typedef struct {
    uint32_t tag;
    uint8_t data[BLOCK_SIZE];
    bool valid;
} CacheLine;

CacheLine l1_cache[L1_CACHE_SIZE];
CacheLine l2_cache[L2_CACHE_SIZE];
uint8_t main_memory[MAIN_MEMORY_SIZE];

uint32_t l1_hits = 0, l1_misses = 0;
uint32_t l2_hits = 0, l2_misses = 0;

uint32_t calculate_tag(uint32_t address, uint32_t cache_size) {
    return address / BLOCK_SIZE / cache_size;
}

uint32_t calculate_index(uint32_t address, uint32_t cache_size) {
    return (address / BLOCK_SIZE) % cache_size;
}

bool check_cache(CacheLine* cache, uint32_t cache_size, uint32_t address, uint8_t* data) {
    uint32_t tag = calculate_tag(address, cache_size);
    uint32_t index = calculate_index(address, cache_size);
    
    if (cache[index].valid && cache[index].tag == tag) {
        memcpy(data, cache[index].data, BLOCK_SIZE);
        return true;
    }
    return false;
}

void update_cache(CacheLine* cache, uint32_t cache_size, uint32_t address, uint8_t* data) {
    uint32_t tag = calculate_tag(address, cache_size);
    uint32_t index = calculate_index(address, cache_size);
    
    cache[index].tag = tag;
    memcpy(cache[index].data, data, BLOCK_SIZE);
    cache[index].valid = true;
}

void memory_access(uint32_t address) {
    uint8_t data[BLOCK_SIZE];
    
    printf("Accessing address: 0x%x\n", address);
    
    if (check_cache(l1_cache, L1_CACHE_SIZE, address, data)) {
        printf("L1 Cache Hit\n");
        l1_hits++;
    } else if (check_cache(l2_cache, L2_CACHE_SIZE, address, data)) {
        printf("L2 Cache Hit\n");
        l2_hits++;
        update_cache(l1_cache, L1_CACHE_SIZE, address, data); // Update L1 on L2 hit
    } else {
        printf("Main Memory Access\n");
        l2_misses++; // Increment L2 miss count since it wasn't in L1 or L2
        uint32_t block_start = address & ~(BLOCK_SIZE - 1);
        if (block_start + BLOCK_SIZE <= MAIN_MEMORY_SIZE) {
            memcpy(data, &main_memory[block_start], BLOCK_SIZE);
            update_cache(l2_cache, L2_CACHE_SIZE, address, data);
            update_cache(l1_cache, L1_CACHE_SIZE, address, data);
        } else {
            printf("Error: Attempted to access memory out of bounds\n");
        }
    }
}

int main() {
    // Initialize memory with some data
    for (int i = 0; i < MAIN_MEMORY_SIZE; i++) {
        main_memory[i] = i & 0xFF;
    }

    // Initialize cache lines
    for (int i = 0; i < L1_CACHE_SIZE; i++) {
        l1_cache[i].valid = false;
    }
    for (int i = 0; i < L2_CACHE_SIZE; i++) {
        l2_cache[i].valid = false;
    }

    // Simulate some memory accesses
    memory_access(0x100);       // Miss in both L1 and L2, loads from main memory
    memory_access(0x101);       // L1 hit
    memory_access(0x200);       // Miss in both L1 and L2, loads from main memory
    memory_access(0x100);       // L1 hit

    // Print hit/miss statistics
    printf("L1 Cache Hits: %u, L1 Cache Misses: %u\n", l1_hits, l1_misses);
    printf("L2 Cache Hits: %u, L2 Cache Misses: %u\n", l2_hits, l2_misses);

    return 0;
}
