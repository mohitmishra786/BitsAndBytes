#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 10000
#define CACHE_SIZE 100
#define PREFETCH_DISTANCE 10

typedef struct {
    int data[CACHE_SIZE];
    int tags[CACHE_SIZE];
    int lru[CACHE_SIZE];
} Cache;

Cache cache;
int main_memory[ARRAY_SIZE];
int prefetch_stride = 0;
int last_address = -1;

void init_memory() {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        main_memory[i] = rand() % 1000;
    }
}

void init_cache() {
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache.tags[i] = -1;
        cache.lru[i] = 0;
    }
}

int find_in_cache(int address) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache.tags[i] == address) {
            return i;
        }
    }
    return -1;
}

int find_lru() {
    int lru_index = 0;
    for (int i = 1; i < CACHE_SIZE; i++) {
        if (cache.lru[i] < cache.lru[lru_index]) {
            lru_index = i;
        }
    }
    return lru_index;
}

void update_lru(int index) {
    cache.lru[index] = time(NULL);
}

void bring_to_cache(int address) {
    int index = find_lru();
    cache.data[index] = main_memory[address];
    cache.tags[index] = address;
    update_lru(index);
}

void prefetch(int address) {
    int prefetch_address = address + prefetch_stride;
    if (prefetch_address < ARRAY_SIZE && find_in_cache(prefetch_address) == -1) {
        bring_to_cache(prefetch_address);
        printf("Prefetched: %d\n", prefetch_address);
    }
}

int access_memory(int address) {
    int cache_index = find_in_cache(address);
    if (cache_index != -1) {
        printf("Cache hit: %d\n", address);
        update_lru(cache_index);
        return cache.data[cache_index];
    } else {
        printf("Cache miss: %d\n", address);
        bring_to_cache(address);
        
        if (last_address != -1) {
            int new_stride = address - last_address;
            if (new_stride == prefetch_stride) {
                for (int i = 1; i <= PREFETCH_DISTANCE; i++) {
                    prefetch(address + i * prefetch_stride);
                }
            } else {
                prefetch_stride = new_stride;
            }
        }
        last_address = address;
        
        return main_memory[address];
    }
}

int main() {
    srand(time(NULL));
    init_memory();
    init_cache();

    for (int i = 0; i < 50; i += 2) {
        access_memory(i);
    }

    return 0;
}
