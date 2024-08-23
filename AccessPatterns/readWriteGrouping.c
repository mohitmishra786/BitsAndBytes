#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>
#include <stdio.h>

#define MAX_QUEUE_SIZE 1024
#define NUM_BANKS 16
#define CACHE_LINE_SIZE 64
#define TLB_SIZE 64

typedef enum {
    READ_MODE,
    WRITE_MODE,
    MODE_TRANSITION
} BusMode;

typedef struct {
    uint64_t address;
    uint64_t data;
    uint32_t priority;
    uint64_t timestamp;
} __attribute__((aligned(CACHE_LINE_SIZE))) MemoryRequest;

typedef struct {
    MemoryRequest* requests;
    int head;
    int tail;
    int size;
    uint64_t total_age;
    _Atomic uint32_t lock;
} __attribute__((aligned(CACHE_LINE_SIZE))) Queue;

typedef struct {
    uint64_t logical_addr;
    uint64_t physical_addr;
    uint64_t last_access;
} TLBEntry;

typedef struct {
    Queue read_queue;
    Queue write_queue;
    BusMode current_mode;
    int read_count;
    int write_count;
    uint64_t* bank_timers;
    TLBEntry tlb[TLB_SIZE];
    uint8_t* data_buffer;
} MemoryController;

void init_queue(Queue* q);
bool enqueue(Queue* q, MemoryRequest* req);
bool dequeue(Queue* q, MemoryRequest* req);
void init_memory_controller(MemoryController* mc);
void process_memory_requests(MemoryController* mc);
uint64_t translate_address(MemoryController* mc, uint64_t logical_addr);
void handle_data_transfer(MemoryController* mc, MemoryRequest* req);

void init_queue(Queue* q) {
    q->requests = aligned_alloc(CACHE_LINE_SIZE, MAX_QUEUE_SIZE * sizeof(MemoryRequest));
    q->head = q->tail = 0;
    q->size = 0;
    q->total_age = 0;
    q->lock = 0;
    printf("Initialized queue at %p\n", (void*)q);
}

bool enqueue(Queue* q, MemoryRequest* req) {
    while (__atomic_test_and_set(&q->lock, __ATOMIC_ACQUIRE));
    
    if (q->size == MAX_QUEUE_SIZE) {
        __atomic_clear(&q->lock, __ATOMIC_RELEASE);
        printf("Queue full, enqueue failed\n");
        return false;
    }
    
    q->requests[q->tail] = *req;
    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
    q->size++;
    q->total_age += req->priority;
    
    __atomic_clear(&q->lock, __ATOMIC_RELEASE);
    printf("Enqueued request: address 0x%lx, priority %u\n", req->address, req->priority);
    return true;
}

bool dequeue(Queue* q, MemoryRequest* req) {
    while (__atomic_test_and_set(&q->lock, __ATOMIC_ACQUIRE));
    
    if (q->size == 0) {
        __atomic_clear(&q->lock, __ATOMIC_RELEASE);
        printf("Queue empty, dequeue failed\n");
        return false;
    }
    
    *req = q->requests[q->head];
    q->head = (q->head + 1) % MAX_QUEUE_SIZE;
    q->size--;
    q->total_age -= req->priority;
    
    __atomic_clear(&q->lock, __ATOMIC_RELEASE);
    printf("Dequeued request: address 0x%lx, priority %u\n", req->address, req->priority);
    return true;
}

void init_memory_controller(MemoryController* mc) {
    init_queue(&mc->read_queue);
    init_queue(&mc->write_queue);
    mc->current_mode = READ_MODE;
    mc->read_count = mc->write_count = 0;
    mc->bank_timers = calloc(NUM_BANKS, sizeof(uint64_t));
    memset(mc->tlb, 0, sizeof(mc->tlb));
    mc->data_buffer = aligned_alloc(CACHE_LINE_SIZE, CACHE_LINE_SIZE * 1024);
    printf("Initialized memory controller\n");
}

void process_memory_requests(MemoryController* mc) {
    MemoryRequest req;
    uint64_t cycle_count = 0;
    
    printf("Starting memory request processing\n");
    while (cycle_count < 1000000) {
        bool should_switch = false;
        
        switch (mc->current_mode) {
            case READ_MODE:
                if (!dequeue(&mc->read_queue, &req)) {
                    should_switch = mc->write_queue.size > 0;
                    printf("No read requests, considering mode switch\n");
                } else {
                    handle_data_transfer(mc, &req);
                    mc->read_count++;
                    should_switch = (mc->read_count >= MAX_QUEUE_SIZE / 2) && (mc->write_queue.size > 0);
                    printf("Processed read request, read_count: %d\n", mc->read_count);
                }
                break;
                
            case WRITE_MODE:
                if (!dequeue(&mc->write_queue, &req)) {
                    should_switch = mc->read_queue.size > 0;
                    printf("No write requests, considering mode switch\n");
                } else {
                    handle_data_transfer(mc, &req);
                    mc->write_count++;
                    should_switch = (mc->write_count >= MAX_QUEUE_SIZE / 2) && (mc->read_queue.size > 0);
                    printf("Processed write request, write_count: %d\n", mc->write_count);
                }
                break;
                
            case MODE_TRANSITION:
                printf("Mode transition: %s -> %s\n", 
                       mc->current_mode == READ_MODE ? "READ" : "WRITE",
                       mc->current_mode == READ_MODE ? "WRITE" : "READ");
                mc->current_mode = (mc->current_mode == READ_MODE) ? WRITE_MODE : READ_MODE;
                mc->read_count = mc->write_count = 0;
                break;
        }
        
        if (should_switch) {
            mc->current_mode = MODE_TRANSITION;
            printf("Initiating mode switch\n");
        }
        
        cycle_count++;
        if (cycle_count % 10000 == 0) {
            printf("Processed %lu cycles\n", cycle_count);
        }
    }
    printf("Completed memory request processing\n");
}

uint64_t translate_address(MemoryController* mc, uint64_t logical_addr) {
    int tlb_index = logical_addr % TLB_SIZE;
    if (mc->tlb[tlb_index].logical_addr == logical_addr) {
        mc->tlb[tlb_index].last_access = __rdtsc();
        printf("TLB hit for address 0x%lx\n", logical_addr);
        return mc->tlb[tlb_index].physical_addr;
    }
    
    uint64_t physical_addr = logical_addr ^ (logical_addr >> 12);
    
    mc->tlb[tlb_index].logical_addr = logical_addr;
    mc->tlb[tlb_index].physical_addr = physical_addr;
    mc->tlb[tlb_index].last_access = __rdtsc();
    
    printf("TLB miss for address 0x%lx, new physical address 0x%lx\n", logical_addr, physical_addr);
    return physical_addr;
}

void handle_data_transfer(MemoryController* mc, MemoryRequest* req) {
    uint64_t physical_addr = translate_address(mc, req->address);
    int bank = physical_addr % NUM_BANKS;
    
    while (__rdtsc() < mc->bank_timers[bank]);
    
    if (req->address & 1) {
        memcpy(&req->data, &mc->data_buffer[physical_addr % (CACHE_LINE_SIZE * 1024)], sizeof(uint64_t));
        printf("Read data 0x%lx from physical address 0x%lx\n", req->data, physical_addr);
    } else {
        memcpy(&mc->data_buffer[physical_addr % (CACHE_LINE_SIZE * 1024)], &req->data, sizeof(uint64_t));
        printf("Wrote data 0x%lx to physical address 0x%lx\n", req->data, physical_addr);
    }
    
    mc->bank_timers[bank] = __rdtsc() + 100;
    printf("Updated bank %d timer, next available at cycle %lu\n", bank, mc->bank_timers[bank]);
}

int main() {
    MemoryController mc;
    init_memory_controller(&mc);
    for (int i = 0; i < 100; i++) {
        MemoryRequest req = {
            .address = rand() & 0xFFFFFFFF,
            .data = rand(),
            .priority = rand() % 10,
            .timestamp = __rdtsc()
        };
        if (req.address & 1) {
            enqueue(&mc.read_queue, &req);
        } else {
            enqueue(&mc.write_queue, &req);
        }
    }
    
    process_memory_requests(&mc);
    return 0;
}
