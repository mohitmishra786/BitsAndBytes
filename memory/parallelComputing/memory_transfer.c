#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CACHE_LINE_SIZE 64
#define L1_CACHE_SIZE (64 * 1024)  // 64KB L1 Cache
#define L2_CACHE_SIZE (6 * 1024 * 1024)  // 6MB L2 Cache

typedef struct {
    unsigned char data[CACHE_LINE_SIZE];
    int valid;
    unsigned long tag;
} CacheLine;

typedef struct {
    CacheLine* lines;
    int size;
} Cache;

Cache* create_cache(int size) {
    Cache* cache = (Cache*)malloc(sizeof(Cache));
    cache->size = size / CACHE_LINE_SIZE;
    cache->lines = (CacheLine*)calloc(cache->size, sizeof(CacheLine));
    return cache;
}

int simulate_memory_access(Cache* l1_cache, Cache* l2_cache, unsigned long address) {
    int l1_index = (address / CACHE_LINE_SIZE) % l1_cache->size;
    int l2_index = (address / CACHE_LINE_SIZE) % l2_cache->size;
    unsigned long tag = address / CACHE_LINE_SIZE;
    
    // Check L1 Cache
    if (l1_cache->lines[l1_index].valid && 
        l1_cache->lines[l1_index].tag == tag) {
        return 1;  // L1 Cache hit
    }
    
    // Check L2 Cache
    if (l2_cache->lines[l2_index].valid && 
        l2_cache->lines[l2_index].tag == tag) {
        // L2 Cache hit, copy to L1
        l1_cache->lines[l1_index].valid = 1;
        l1_cache->lines[l1_index].tag = tag;
        memcpy(l1_cache->lines[l1_index].data,
               l2_cache->lines[l2_index].data,
               CACHE_LINE_SIZE);
        return 2;  // L2 Cache hit
    }
    
    // Cache miss, load from main memory
    l2_cache->lines[l2_index].valid = 1;
    l2_cache->lines[l2_index].tag = tag;
    // Simulate memory load
    for (int i = 0; i < CACHE_LINE_SIZE; i++) {
        l2_cache->lines[l2_index].data[i] = rand() % 256;
    }
    
    // Copy to L1
    l1_cache->lines[l1_index].valid = 1;
    l1_cache->lines[l1_index].tag = tag;
    memcpy(l1_cache->lines[l1_index].data,
           l2_cache->lines[l2_index].data,
           CACHE_LINE_SIZE);
    
    return 3;  // Cache miss
}

int main() {
    Cache* l1_cache = create_cache(L1_CACHE_SIZE);
    Cache* l2_cache = create_cache(L2_CACHE_SIZE);
    
    srand(time(NULL));
    
    // Simulate memory accesses
    int num_accesses = 1000000;
    int l1_hits = 0, l2_hits = 0, misses = 0;
    
    for (int i = 0; i < num_accesses; i++) {
        unsigned long address = rand() % (1ULL << 32);  // 32-bit address space
        int result = simulate_memory_access(l1_cache, l2_cache, address);
        
        switch(result) {
            case 1: l1_hits++; break;
            case 2: l2_hits++; break;
            case 3: misses++; break;
        }
    }
    
    printf("Memory Access Statistics:\n");
    printf("L1 Cache Hits: %d (%.2f%%)\n", 
           l1_hits, (float)l1_hits/num_accesses*100);
    printf("L2 Cache Hits: %d (%.2f%%)\n", 
           l2_hits, (float)l2_hits/num_accesses*100);
    printf("Cache Misses: %d (%.2f%%)\n", 
           misses, (float)misses/num_accesses*100);
    
    free(l1_cache->lines);
    free(l1_cache);
    free(l2_cache->lines);
    free(l2_cache);
    
    return 0;
}