#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define PAGE_SIZE 4096
#define BUFFER_POOL_SIZE 10
#define MAX_PAGES 100

typedef struct {
    int page_id;
    char *data;
    int dirty;
    int pin_count;
    time_t last_access;
} BufferFrame;

typedef struct {
    BufferFrame frames[BUFFER_POOL_SIZE];
    int file_descriptor;
    int page_faults;
    int disk_writes;
} BufferPool;

BufferPool *create_buffer_pool(const char *filename) {
    BufferPool *pool = (BufferPool*)malloc(sizeof(BufferPool));
    pool->file_descriptor = open(filename, O_RDWR | O_CREAT, 0644);
    pool->page_faults = 0;
    pool->disk_writes = 0;
    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        pool->frames[i].page_id = -1;
        pool->frames[i].data = (char*)malloc(PAGE_SIZE);
        pool->frames[i].dirty = 0;
        pool->frames[i].pin_count = 0;
        pool->frames[i].last_access = 0;
    }
    printf("Buffer pool created with %d frames\n", BUFFER_POOL_SIZE);
    return pool;
}

int find_lru_page(BufferPool *pool) {
    int lru_index = 0;
    time_t oldest_time = pool->frames[0].last_access;

    for (int i = 1; i < BUFFER_POOL_SIZE; i++) {
        if (pool->frames[i].pin_count == 0 && pool->frames[i].last_access < oldest_time) {
            oldest_time = pool->frames[i].last_access;
            lru_index = i;
        }
    }

    return lru_index;
}

BufferFrame *get_page(BufferPool *pool, int page_id) {
    printf("Requesting page %d\n", page_id);

    // Check if page is already in buffer pool
    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        if (pool->frames[i].page_id == page_id) {
            pool->frames[i].pin_count++;
            pool->frames[i].last_access = time(NULL);
            printf("Page %d found in frame %d\n", page_id, i);
            return &pool->frames[i];
        }
    }

    // Page not in buffer pool, need to load it
    pool->page_faults++;
    printf("Page fault occurred for page %d\n", page_id);

    int victim_frame = find_lru_page(pool);
    BufferFrame *frame = &pool->frames[victim_frame];

    // Write dirty page back to disk if necessary
    if (frame->dirty) {
        lseek(pool->file_descriptor, frame->page_id * PAGE_SIZE, SEEK_SET);
        write(pool->file_descriptor, frame->data, PAGE_SIZE);
        pool->disk_writes++;
        printf("Writing dirty page %d back to disk\n", frame->page_id);
    }

    // Load new page from disk
    lseek(pool->file_descriptor, page_id * PAGE_SIZE, SEEK_SET);
    read(pool->file_descriptor, frame->data, PAGE_SIZE);

    frame->page_id = page_id;
    frame->dirty = 0;
    frame->pin_count = 1;
    frame->last_access = time(NULL);

    printf("Loaded page %d into frame %d\n", page_id, victim_frame);
    return frame;
}

void release_page(BufferFrame *frame) {
    frame->pin_count--;
    printf("Released page %d, pin count now %d\n", frame->page_id, frame->pin_count);
}

void mark_dirty(BufferFrame *frame) {
    frame->dirty = 1;
    printf("Marked page %d as dirty\n", frame->page_id);
}

void flush_page(BufferPool *pool, BufferFrame *frame) {
    if (frame->dirty) {
        lseek(pool->file_descriptor, frame->page_id * PAGE_SIZE, SEEK_SET);
        write(pool->file_descriptor, frame->data, PAGE_SIZE);
        frame->dirty = 0;
        pool->disk_writes++;
        printf("Flushed dirty page %d to disk\n", frame->page_id);
    }
}

void destroy_buffer_pool(BufferPool *pool) {
    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        if (pool->frames[i].dirty) {
            flush_page(pool, &pool->frames[i]);
        }
        free(pool->frames[i].data);
    }
    close(pool->file_descriptor);
    printf("Buffer pool destroyed. Total page faults: %d, Total disk writes: %d\n",
           pool->page_faults, pool->disk_writes);
    free(pool);
}

int main() {
    BufferPool *pool = create_buffer_pool("database.db");

    // Simulate a query that needs pages 1, 5, and 10
    BufferFrame *page1 = get_page(pool, 1);
    BufferFrame *page5 = get_page(pool, 5);
    BufferFrame *page10 = get_page(pool, 10);

    // Simulate some operations on the pages
    strcpy(page1->data, "Updated data for page 1");
    mark_dirty(page1);

    // Release the pages when done
    release_page(page1);
    release_page(page5);
    release_page(page10);

    // Simulate another query that needs pages 2, 5, and 15
    BufferFrame *page2 = get_page(pool, 2);
    page5 = get_page(pool, 5);  // This should already be in memory
    BufferFrame *page15 = get_page(pool, 15);

    // Simulate some operations on the pages
    strcpy(page2->data, "New data for page 2");
    mark_dirty(page2);

    // Release the pages when done
    release_page(page2);
    release_page(page5);
    release_page(page15);

    // Clean up
    destroy_buffer_pool(pool);

    return 0;
}
