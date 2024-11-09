#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MEMORY_SIZE (1024 * 1024)
#define CACHE_SIZE (64 * 1024)
#define CACHE_LINE_SIZE 64
#define NUM_CACHE_LINES (CACHE_SIZE / CACHE_LINE_SIZE)
#define MAX_CACHE_SETS 4

typedef enum { Read, Write } AccessMode;
typedef enum { LRU, Random } ReplacementPolicy;

typedef struct CacheLine {
    unsigned char data[CACHE_LINE_SIZE];
    size_t tag;
    int dirty;
    clock_t last_used;
} CacheLine;

typedef struct MemoryController {
    unsigned char *main_memory;
    CacheLine ***cache;
    ReplacementPolicy replacement_policy;
    unsigned long rng_seed;
} MemoryController;

MemoryController* MemoryController_new();
void MemoryController_free(MemoryController *mc);
void MemoryController_access(MemoryController *mc, size_t address, AccessMode mode, unsigned char data);
void MemoryController_setReplacementPolicy(MemoryController *mc, ReplacementPolicy policy);
void MemoryController_displayCacheStatus(MemoryController *mc);

MemoryController* MemoryController_new() {
    MemoryController *mc = (MemoryController *)malloc(sizeof(MemoryController));
    mc->main_memory = (unsigned char *)malloc(MEMORY_SIZE * sizeof(unsigned char));
    mc->cache = (CacheLine ***)malloc(NUM_CACHE_LINES * sizeof(CacheLine **));
    for (int i = 0; i < NUM_CACHE_LINES; i++) {
        mc->cache[i] = (CacheLine **)malloc(MAX_CACHE_SETS * sizeof(CacheLine *));
        for (int j = 0; j < MAX_CACHE_SETS; j++) {
            mc->cache[i][j] = NULL;
        }
    }
    mc->replacement_policy = LRU;
    mc->rng_seed = (unsigned long)time(NULL);
    printf("Memory Controller initialized with %zu bytes of main memory and %zu bytes of cache.\n", MEMORY_SIZE, CACHE_SIZE);
    return mc;
}

void MemoryController_free(MemoryController *mc) {
    for (int i = 0; i < NUM_CACHE_LINES; i++) {
        for (int j = 0; j < MAX_CACHE_SETS; j++) {
            if (mc->cache[i][j]) free(mc->cache[i][j]);
        }
        free(mc->cache[i]);
    }
    free(mc->cache);
    free(mc->main_memory);
    free(mc);
}

void MemoryController_access(MemoryController *mc, size_t address, AccessMode mode, unsigned char data) {
    size_t cache_line = (address / CACHE_LINE_SIZE) % NUM_CACHE_LINES;
    size_t tag = address / CACHE_LINE_SIZE;
    size_t offset = address % CACHE_LINE_SIZE;

    int cache_hit = 0;
    for (int i = 0; i < MAX_CACHE_SETS; i++) {
        if (mc->cache[cache_line][i] && mc->cache[cache_line][i]->tag == tag) {
            cache_hit = 1;
            mc->cache[cache_line][i]->last_used = clock();
            if (mode == Read) {
                printf("Cache hit! Read data: %d\n", mc->cache[cache_line][i]->data[offset]);
            } else {
                mc->cache[cache_line][i]->data[offset] = data;
                mc->cache[cache_line][i]->dirty = 1;
                printf("Cache hit! Wrote data: %d\n", data);
            }
            break;
        }
    }

    if (!cache_hit) {
        printf("Cache miss. ");
        CacheLine *new_entry = (CacheLine *)malloc(sizeof(CacheLine));
        new_entry->tag = tag;
        memcpy(new_entry->data, &mc->main_memory[address - offset], CACHE_LINE_SIZE);

        int empty_slot = -1;
        for (int i = 0; i < MAX_CACHE_SETS; i++) {
            if (mc->cache[cache_line][i] == NULL) {
                empty_slot = i;
                break;
            }
        }

        if (empty_slot != -1) {
            mc->cache[cache_line][empty_slot] = new_entry;
        } else {
            CacheLine **victim = &mc->cache[cache_line][0];
            if (mc->replacement_policy == LRU) {
                for (int i = 1; i < MAX_CACHE_SETS; i++) {
                    if (mc->cache[cache_line][i]->last_used < (*victim)->last_used) {
                        victim = &mc->cache[cache_line][i];
                    }
                }
            } else { // Random
                srand(mc->rng_seed);
                victim = &mc->cache[cache_line][rand() % MAX_CACHE_SETS];
                mc->rng_seed++;
            }

            if ((*victim)->dirty) {
                memcpy(&mc->main_memory[((*victim)->tag * NUM_CACHE_LINES + cache_line) * CACHE_LINE_SIZE], (*victim)->data, CACHE_LINE_SIZE);
                printf("Writing back dirty cache line to main memory at address %zu\n", ((*victim)->tag * NUM_CACHE_LINES + cache_line) * CACHE_LINE_SIZE);
            }
            free(*victim);
            *victim = new_entry;
        }

        if (mode == Read) {
            printf("Read data: %d\n", new_entry->data[offset]);
        } else {
            new_entry->data[offset] = data;
            new_entry->dirty = 1;
            printf("Wrote data: %d\n", data);
        }
    }
}

void MemoryController_setReplacementPolicy(MemoryController *mc, ReplacementPolicy policy) {
    mc->replacement_policy = policy;
    printf("Replacement policy set to %s\n", policy == LRU ? "LRU" : "Random");
}

void MemoryController_displayCacheStatus(MemoryController *mc) {
    printf("\nCache Status:\n");
    for (size_t i = 0; i < NUM_CACHE_LINES; ++i) {
        printf("Set %zu: ", i);
        for (int j = 0; j < MAX_CACHE_SETS; j++) {
            if (mc->cache[i][j]) {
                printf("[Tag: %zu, Dirty: %s] ", mc->cache[i][j]->tag, mc->cache[i][j]->dirty ? "Yes" : "No");
            } else {
                printf("[Empty] ");
            }
        }
        printf("\n");
    }
}

int main() {
    MemoryController *mc = MemoryController_new();

    MemoryController_access(mc, 0, Write, 42);
    MemoryController_access(mc, 0, Read, 0);
    MemoryController_access(mc, 64, Write, 100);
    MemoryController_access(mc, 128, Write, 200);
    MemoryController_access(mc, 0, Read, 0);

    MemoryController_displayCacheStatus(mc);

    // Change replacement policy
    MemoryController_setReplacementPolicy(mc, Random);

    MemoryController_access(mc, 1024, Write, 50);
    MemoryController_access(mc, 2048, Write, 75);
    MemoryController_access(mc, 64, Read, 0);

    MemoryController_displayCacheStatus(mc);

    MemoryController_free(mc);
    return 0;
}
