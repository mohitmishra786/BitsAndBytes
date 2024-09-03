#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define PAGE_SIZE 4096
#define PAGE_TABLE_ENTRIES 1024
#define PHYSICAL_MEMORY_SIZE (100 * PAGE_SIZE)
#define DISK_SIZE (1000 * PAGE_SIZE)
#define TLB_SIZE 16

typedef struct {
    unsigned int frame_number : 20;
    unsigned int present : 1;
    unsigned int dirty : 1;
    unsigned int accessed : 1;
} PageTableEntry;

typedef struct {
    unsigned int page_number;
    unsigned int frame_number;
} TLBEntry;

unsigned char physical_memory[PHYSICAL_MEMORY_SIZE];
unsigned char disk[DISK_SIZE];
PageTableEntry page_table[PAGE_TABLE_ENTRIES];
TLBEntry tlb[TLB_SIZE];
int tlb_count = 0;

void init_memory() {
    memset(page_table, 0, sizeof(page_table));
    memset(tlb, 0, sizeof(tlb));
    memset(physical_memory, 0, sizeof(physical_memory));
    memset(disk, 0, sizeof(disk));
}

unsigned int translate_address(unsigned int virtual_address) {
    unsigned int page_number = virtual_address / PAGE_SIZE;
    unsigned int offset = virtual_address % PAGE_SIZE;
    
    // Check TLB first
    for (int i = 0; i < tlb_count; i++) {
        if (tlb[i].page_number == page_number) {
            printf("TLB hit!\n");
            return tlb[i].frame_number * PAGE_SIZE + offset;
        }
    }
    
    printf("TLB miss. Checking page table...\n");
    
    // TLB miss, check page table
    PageTableEntry pte = page_table[page_number];
    
    if (pte.present) {
        printf("Page found in memory.\n");
        
        // Update TLB
        if (tlb_count < TLB_SIZE) {
            tlb[tlb_count].page_number = page_number;
            tlb[tlb_count].frame_number = pte.frame_number;
            tlb_count++;
        } else {
            // TLB is full, replace an entry (better to use simple FIFO for this example)
            for (int i = 0; i < TLB_SIZE - 1; i++) {
                tlb[i] = tlb[i + 1];
            }
            tlb[TLB_SIZE - 1].page_number = page_number;
            tlb[TLB_SIZE - 1].frame_number = pte.frame_number;
        }
        
        return pte.frame_number * PAGE_SIZE + offset;
    } else {
        printf("Page fault! Loading page from disk...\n");
        
        // Find a free frame (simplified, in real world it is much more complex)
        int free_frame = -1;
        for (int i = 0; i < PHYSICAL_MEMORY_SIZE / PAGE_SIZE; i++) {
            if (physical_memory[i * PAGE_SIZE] == 0) {
                free_frame = i;
                break;
            }
        }
        
        if (free_frame == -1) {
            printf("No free frames available. Implementing page replacement...\n");
            // We can Implement page replacement algorithm here (e.g., FIFO, LRU)
            // For simplicity, we'll just use the first frame
            free_frame = 0;
        }
        
        // Load page from disk to memory
        memcpy(&physical_memory[free_frame * PAGE_SIZE], &disk[page_number * PAGE_SIZE], PAGE_SIZE);
        
        // Update page table
        page_table[page_number].frame_number = free_frame;
        page_table[page_number].present = 1;
        page_table[page_number].accessed = 1;
        
        // Update TLB
        if (tlb_count < TLB_SIZE) {
            tlb[tlb_count].page_number = page_number;
            tlb[tlb_count].frame_number = free_frame;
            tlb_count++;
        } else {
            // TLB is full, replace an entry (simple FIFO for this example)
            for (int i = 0; i < TLB_SIZE - 1; i++) {
                tlb[i] = tlb[i + 1];
            }
            tlb[TLB_SIZE - 1].page_number = page_number;
            tlb[TLB_SIZE - 1].frame_number = free_frame;
        }
        
        return free_frame * PAGE_SIZE + offset;
    }
}

void write_memory(unsigned int virtual_address, unsigned char value) {
    unsigned int physical_address = translate_address(virtual_address);
    physical_memory[physical_address] = value;
    
    // Mark page as dirty
    unsigned int page_number = virtual_address / PAGE_SIZE;
    page_table[page_number].dirty = 1;
}

unsigned char read_memory(unsigned int virtual_address) {
    unsigned int physical_address = translate_address(virtual_address);
    return physical_memory[physical_address];
}

int main() {
    init_memory();
    
    // Simulate some memory accesses
    write_memory(0, 'A');
    write_memory(4096, 'B');
    write_memory(8192, 'C');
    
    printf("Value at address 0: %c\n", read_memory(0));
    printf("Value at address 4096: %c\n", read_memory(4096));
    printf("Value at address 8192: %c\n", read_memory(8192));
    
    return 0;
}
