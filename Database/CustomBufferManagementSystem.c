#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

#define PAGE_SIZE 4096
#define BUFFER_POOL_SIZE 1024
#define PAGE_TABLE_SIZE 2048

typedef struct {
    char data[PAGE_SIZE];
    int page_id;
    bool is_dirty;
    int pin_count;
    struct timeval last_used;
} Page;

typedef struct {
    int page_id;
    int frame_id;
} PageTableEntry;

typedef struct FreeListNode {
    int frame_id;
    struct FreeListNode* next;
} FreeListNode;

void init_buffer_pool();
Page* fetch_page(int page_id);
void unpin_page(int page_id);
void mark_page_dirty(int page_id);
void flush_dirty_pages();
void print_buffer_pool_stats();
void print_page_table();

#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Page buffer_pool[BUFFER_POOL_SIZE];
PageTableEntry page_table[PAGE_TABLE_SIZE];
FreeListNode* free_list_head = NULL;
int clock_hand = 0;
bool reference_bits[BUFFER_POOL_SIZE];

void init_buffer_pool() {
    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        buffer_pool[i].page_id = -1;
        buffer_pool[i].is_dirty = false;
        buffer_pool[i].pin_count = 0;
        
        FreeListNode* node = malloc(sizeof(FreeListNode));
        node->frame_id = i;
        node->next = free_list_head;
        free_list_head = node;
        
        reference_bits[i] = false;
    }
    
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        page_table[i].page_id = -1;
        page_table[i].frame_id = -1;
    }
}

int hash_function(int page_id) {
    return page_id % PAGE_TABLE_SIZE;
}

int lookup_page(int page_id) {
    int hash = hash_function(page_id);
    while (page_table[hash].page_id != -1) {
        if (page_table[hash].page_id == page_id) {
            return page_table[hash].frame_id;
        }
        hash = (hash + 1) % PAGE_TABLE_SIZE;
    }
    return -1; // Page not found
}

void insert_into_page_table(int page_id, int frame_id) {
    int hash = hash_function(page_id);
    while (page_table[hash].page_id != -1) {
        hash = (hash + 1) % PAGE_TABLE_SIZE;
    }
    page_table[hash].page_id = page_id;
    page_table[hash].frame_id = frame_id;
}

void remove_from_page_table(int page_id) {
    int hash = hash_function(page_id);
    while (page_table[hash].page_id != page_id) {
        hash = (hash + 1) % PAGE_TABLE_SIZE;
    }
    page_table[hash].page_id = -1;
    page_table[hash].frame_id = -1;
}

int find_victim_page() {
    while (true) {
        if (buffer_pool[clock_hand].pin_count == 0) {
            if (reference_bits[clock_hand]) {
                reference_bits[clock_hand] = false;
            } else {
                int victim = clock_hand;
                clock_hand = (clock_hand + 1) % BUFFER_POOL_SIZE;
                return victim;
            }
        }
        clock_hand = (clock_hand + 1) % BUFFER_POOL_SIZE;
    }
}

void read_page_from_disk(int page_id, char* buffer) {
    printf("Reading page %d from disk\n", page_id);
    memset(buffer, 0, PAGE_SIZE);
    snprintf(buffer, PAGE_SIZE, "Data for page %d", page_id);
}

void write_page_to_disk(int page_id, char* buffer) {
    printf("Writing page %d to disk\n", page_id);
}

Page* fetch_page(int page_id) {
    int frame_id = lookup_page(page_id);
    if (frame_id != -1) {
        // Page hit
        buffer_pool[frame_id].pin_count++;
        gettimeofday(&buffer_pool[frame_id].last_used, NULL);
        reference_bits[frame_id] = true;
        return &buffer_pool[frame_id];
    }
    
    // Page fault
    if (free_list_head) {
        // Use a free frame
        frame_id = free_list_head->frame_id;
        FreeListNode* temp = free_list_head;
        free_list_head = free_list_head->next;
        free(temp);
    } else {
        // Evict a page
        frame_id = find_victim_page();
        if (buffer_pool[frame_id].is_dirty) {
            write_page_to_disk(buffer_pool[frame_id].page_id, buffer_pool[frame_id].data);
        }
        remove_from_page_table(buffer_pool[frame_id].page_id);
    }
    
    // Load new page
    read_page_from_disk(page_id, buffer_pool[frame_id].data);
    buffer_pool[frame_id].page_id = page_id;
    buffer_pool[frame_id].is_dirty = false;
    buffer_pool[frame_id].pin_count = 1;
    gettimeofday(&buffer_pool[frame_id].last_used, NULL);
    reference_bits[frame_id] = true;
    
    insert_into_page_table(page_id, frame_id);
    
    return &buffer_pool[frame_id];
}

void unpin_page(int page_id) {
    int frame_id = lookup_page(page_id);
    if (frame_id != -1 && buffer_pool[frame_id].pin_count > 0) {
        buffer_pool[frame_id].pin_count--;
    }
}

void mark_page_dirty(int page_id) {
    int frame_id = lookup_page(page_id);
    if (frame_id != -1) {
        buffer_pool[frame_id].is_dirty = true;
    }
}

void flush_dirty_pages() {
    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        if (buffer_pool[i].is_dirty) {
            write_page_to_disk(buffer_pool[i].page_id, buffer_pool[i].data);
            buffer_pool[i].is_dirty = false;
        }
    }
}

void print_buffer_pool_stats() {
    int used_frames = 0;
    int dirty_pages = 0;
    int pinned_pages = 0;

    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        if (buffer_pool[i].page_id != -1) {
            used_frames++;
            if (buffer_pool[i].is_dirty) {
                dirty_pages++;
            }
            if (buffer_pool[i].pin_count > 0) {
                pinned_pages++;
            }
        }
    }

    printf("Buffer Pool Stats:\n");
    printf("Used Frames: %d/%d\n", used_frames, BUFFER_POOL_SIZE);
    printf("Dirty Pages: %d\n", dirty_pages);
    printf("Pinned Pages: %d\n", pinned_pages);
}

void print_page_table() {
    printf("Page Table:\n");
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        if (page_table[i].page_id != -1) {
            printf("Hash %d: Page ID %d -> Frame %d\n", i, page_table[i].page_id, page_table[i].frame_id);
        }
    }
}

int main() {
    init_buffer_pool();

    // Test scenario
    for (int i = 0; i < BUFFER_POOL_SIZE + 10; i++) {
        Page* page = fetch_page(i);
        printf("Fetched page %d\n", i);
        if (i % 3 == 0) {
            mark_page_dirty(i);
        }
        if (i % 5 != 0) {
            unpin_page(i);
        }
    }

    print_buffer_pool_stats();
    print_page_table();

    flush_dirty_pages();

    return 0;
}
