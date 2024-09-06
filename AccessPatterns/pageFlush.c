#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>  // Include for PRIu64 and PRIx64 macros

#define PAGE_SIZE 4096
#define PTE_PRESENT 0x1
#define PTE_WRITABLE 0x2
#define PTE_USER 0x4
#define MAX_PROCESSES 256
#define TLB_SIZE 64

typedef uint64_t pte_t;
typedef uint64_t pfn_t;
typedef uint8_t asid_t;

struct page {
    pfn_t pfn;
    int pin_count;
};

struct tlb_entry {
    uint64_t vpn;
    pfn_t pfn;
    asid_t asid;
    bool valid;
};

struct process {
    pte_t *page_table;
    asid_t asid;
};

struct mmu {
    struct tlb_entry tlb[TLB_SIZE];
    asid_t current_asid;
};

struct system {
    struct page *memory;
    size_t memory_size;
    struct process processes[MAX_PROCESSES];
    struct mmu mmu;
    asid_t next_asid;
};

void init_system(struct system *sys, size_t memory_size) {
    sys->memory_size = memory_size;
    sys->memory = (struct page *)calloc(memory_size / PAGE_SIZE, sizeof(struct page));
    if (sys->memory == NULL) {
        printf("Failed to allocate memory for system.\n");
        exit(1);
    }
    sys->next_asid = 1;
    memset(sys->mmu.tlb, 0, sizeof(sys->mmu.tlb));
    printf("System initialized with %zu bytes of memory.\n", memory_size);
}

pfn_t allocate_page(struct system *sys) {
    for (size_t i = 0; i < sys->memory_size / PAGE_SIZE; i++) {
        if (sys->memory[i].pin_count == 0) {
            sys->memory[i].pin_count = 1;
            printf("Allocated page at index %zu\n", i);
            return i;
        }
    }
    printf("Out of memory\n");
    return (pfn_t)-1;
}

void pin_page(struct system *sys, pfn_t pfn) {
    sys->memory[pfn].pin_count++;
    printf("Pinned page %" PRIu64 ", new pin count: %d\n", pfn, sys->memory[pfn].pin_count);
}

void unpin_page(struct system *sys, pfn_t pfn) {
    if (sys->memory[pfn].pin_count > 0) {
        sys->memory[pfn].pin_count--;
        printf("Unpinned page %" PRIu64 ", new pin count: %d\n", pfn, sys->memory[pfn].pin_count);
    } else {
        printf("Attempted to unpin an unpinned page %" PRIu64 "\n", pfn);
    }
}

void flush_tlb_entry(struct mmu *mmu, uint64_t vpn, asid_t asid) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (mmu->tlb[i].vpn == vpn && mmu->tlb[i].asid == asid) {
            mmu->tlb[i].valid = false;
            printf("Flushed TLB entry for VPN %" PRIu64 ", ASID %u\n", vpn, asid);
            return;
        }
    }
    printf("No matching TLB entry found for VPN %" PRIu64 ", ASID %u\n", vpn, asid);
}

void flush_tlb(struct mmu *mmu) {
    for (int i = 0; i < TLB_SIZE; i++) {
        mmu->tlb[i].valid = false;
    }
    printf("Full TLB flush performed\n");
}

uint64_t translate_address(struct system *sys, uint64_t vaddr, asid_t asid) {
    uint64_t vpn = vaddr / PAGE_SIZE;
    uint64_t offset = vaddr % PAGE_SIZE;

    for (int i = 0; i < TLB_SIZE; i++) {
        if (sys->mmu.tlb[i].valid && sys->mmu.tlb[i].vpn == vpn && sys->mmu.tlb[i].asid == asid) {
            printf("TLB hit for VPN %" PRIu64 ", ASID %u\n", vpn, asid);
            return (sys->mmu.tlb[i].pfn * PAGE_SIZE) | offset;
        }
    }

    printf("TLB miss for VPN %" PRIu64 ", ASID %u\n", vpn, asid);

    pte_t pte = sys->processes[asid].page_table[vpn];
    if (!(pte & PTE_PRESENT)) {
        pfn_t new_pfn = allocate_page(sys);
        if (new_pfn == (pfn_t)-1) {
            printf("Failed to allocate page for VPN %" PRIu64 "\n", vpn);
            return (uint64_t)-1;
        }
        sys->processes[asid].page_table[vpn] = (new_pfn << 12) | PTE_PRESENT | PTE_WRITABLE | PTE_USER;
        pte = sys->processes[asid].page_table[vpn];
        printf("New page allocated for VPN %" PRIu64 "\n", vpn);
    }

    for (int i = 0; i < TLB_SIZE; i++) {
        if (!sys->mmu.tlb[i].valid) {
            sys->mmu.tlb[i].vpn = vpn;
            sys->mmu.tlb[i].pfn = pte >> 12;
            sys->mmu.tlb[i].asid = asid;
            sys->mmu.tlb[i].valid = true;
            printf("Updated TLB entry for VPN %" PRIu64 ", ASID %u\n", vpn, asid);
            break;
        }
    }

    return ((pte >> 12) * PAGE_SIZE) | offset;
}

void switch_process(struct system *sys, int new_process_id) {
    if (sys->processes[new_process_id].asid == 0) {
        sys->processes[new_process_id].asid = sys->next_asid++;
        if (sys->next_asid == 0) {
            flush_tlb(&sys->mmu);
            sys->next_asid = 1;
        }
        printf("Assigned ASID %u to process %d\n", sys->processes[new_process_id].asid, new_process_id);
    }
    sys->mmu.current_asid = sys->processes[new_process_id].asid;
    printf("Switched to process %d with ASID %u\n", new_process_id, sys->mmu.current_asid);
}

int main() {
    struct system sys;
    init_system(&sys, 1024 * 1024 * 1024);

    switch_process(&sys, 1);
    uint64_t phys_addr = translate_address(&sys, 0x1000, sys.mmu.current_asid);
    printf("Translated address 0x1000 to physical address 0x%" PRIx64 "\n", phys_addr);
    
    return 0;
}
