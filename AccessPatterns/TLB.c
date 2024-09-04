#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define TLB_SIZE 8
#define MAX_VIRTUAL_ADDRESS 1024 // For simplicity, we'll use a small address space
#define PAGE_SIZE 4096 // 4KB page size

typedef struct {
    int virtual_page;
    int physical_frame;
    bool valid;
} TLBEntry;

TLBEntry TLB[TLB_SIZE];

void initTLB() {
    for (int i = 0; i < TLB_SIZE; i++) {
        TLB[i].valid = false;
    }
}

// to check if TLB has an entry for given virtual page
bool TLBHit(int virtual_page) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (TLB[i].valid && TLB[i].virtual_page == virtual_page) {
            printf("TLB Hit for virtual page %d\n", virtual_page);
            return true;
        }
    }
    printf("TLB Miss for virtual page %d\n", virtual_page);
    return false;
}

//to update TLB with a new entry (simple replacement policy)
void updateTLB(int virtual_page, int physical_frame) {
    int replace_index = rand() % TLB_SIZE; // Random replacement for simplicity
    TLB[replace_index].virtual_page = virtual_page;
    TLB[replace_index].physical_frame = physical_frame;
    TLB[replace_index].valid = true;
    printf("Updated TLB with virtual page %d -> physical frame %d\n", virtual_page, physical_frame);
}

int translateAddress(int virtual_address) {
    int virtual_page = virtual_address / PAGE_SIZE;
    int offset = virtual_address % PAGE_SIZE;

    if (TLBHit(virtual_page)) {
        // For this basic implementation, we'll assume we know the physical frame for demonstration
        int physical_frame = virtual_page; // In real systems, this would come from the TLB
        return (physical_frame * PAGE_SIZE) + offset;
    } else {
        // Here, we'll just use the virtual page number as the physical frame for simplicity
        int physical_frame = virtual_page;
        updateTLB(virtual_page, physical_frame);
        return (physical_frame * PAGE_SIZE) + offset;
    }
}

int main() {
    srand(time(NULL)); // For random replacement
    initTLB();

    int virtual_addresses[] = {1000, 2000, 3000, 4000, 1000, 2000, 5000, 6000};
    int num_addresses = sizeof(virtual_addresses) / sizeof(virtual_addresses[0]);

    for (int i = 0; i < num_addresses; i++) {
        int physical_address = translateAddress(virtual_addresses[i]);
        printf("Translated virtual address %d to physical address %d\n", virtual_addresses[i], physical_address);
        printf("\n");
    }

    return 0;
}
