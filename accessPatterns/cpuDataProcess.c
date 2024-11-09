#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#define MAX_PAGES 1024
#define DRAM_ROWS 8192
#define DRAM_COLS 512
#define BUS_WIDTH 64 
#define CLOCK_SPEED 800000000

typedef struct {
    uint32_t physical_page_frame;
    bool valid;
} page_table_entry_t;

// MAR and MDR registers
uint32_t mar;
uint32_t mdr;

// Page table
page_table_entry_t page_table[MAX_PAGES];

// Simulating DRAM (using a 2D array)
uint32_t** dram;

void activate_ras(uint32_t row_address);
void activate_cas(uint32_t col_address);
uint32_t read_dram_data(void);
void write_dram_data(uint32_t data);

uint32_t translate_address(uint32_t logical_address) {
    printf("Translating logical address: 0x%08X\n", logical_address);
    uint32_t page_number = (logical_address >> 12) & 0x3FF;
    uint32_t page_offset = logical_address & 0xFFF;
    if (page_table[page_number].valid) {
        uint32_t physical_address = (page_table[page_number].physical_page_frame << 12) | page_offset;
        printf("Physical address: 0x%08X\n", physical_address);
        return physical_address;
    } else {
        printf("Page fault occurred\n");
        return 0xFFFFFFFF;
    }
}

void select_row_column(uint32_t physical_address) {
    printf("Selecting row and column in DRAM\n");
    uint32_t row_address = (physical_address >> 9) & 0x1FFF;
    uint32_t col_address = physical_address & 0x1FF;
    activate_ras(row_address);
    activate_cas(col_address);
}

double calculate_bandwidth() {
    printf("Calculating bandwidth\n");
    double bandwidth = (double)BUS_WIDTH * CLOCK_SPEED / 8;
    return bandwidth / 1000000000;
}

void read_from_dram(uint32_t physical_address) {
    printf("Reading data from DRAM\n");
    mar = physical_address;
    select_row_column(physical_address);
    mdr = read_dram_data();
    printf("Data read: 0x%08X\n", mdr);
}

void write_to_dram(uint32_t physical_address, uint32_t data) {
    printf("Writing data to DRAM\n");
    mar = physical_address;
    mdr = data;
    select_row_column(physical_address);
    write_dram_data(mdr);
    printf("Data written: 0x%08X\n", mdr);
}

int main() {
    for (int i = 0; i < MAX_PAGES; i++) {
        page_table[i].physical_page_frame = i;
        page_table[i].valid = true;
    }

    dram = (uint32_t**)malloc(DRAM_ROWS * sizeof(uint32_t*));
    for (int i = 0; i < DRAM_ROWS; i++) {
        dram[i] = (uint32_t*)malloc(DRAM_COLS * sizeof(uint32_t));
    }

    uint32_t logical_address = 0x12345678;
    uint32_t physical_address = translate_address(logical_address);

    read_from_dram(physical_address);
    write_to_dram(physical_address, 0xDEADBEEF);

    double bandwidth = calculate_bandwidth();
    printf("Bandwidth: %.2f GB/s\n", bandwidth);

    for (int i = 0; i < DRAM_ROWS; i++) {
        free(dram[i]);
    }
    free(dram);

    return 0;
}

void activate_ras(uint32_t row_address) {
    printf("  Activating row: %u\n", row_address);
}

void activate_cas(uint32_t col_address) {
    printf("  Activating column: %u\n", col_address);
}

uint32_t read_dram_data(void) {
    uint32_t row_address = (mar >> 9) & 0x1FFF;
    uint32_t col_address = mar & 0x1FF;
    return dram[row_address][col_address];
}

void write_dram_data(uint32_t data) {
    uint32_t row_address = (mar >> 9) & 0x1FFF;
    uint32_t col_address = mar & 0x1FF;
    dram[row_address][col_address] = data;
}
