#include <stdint.h>
#include <stdio.h>

#define BANK_BITS 2
#define ROW_BITS 14
#define COL_BITS 10

typedef struct {
    uint32_t bank;
    uint32_t row;
    uint32_t column;
} DRAMAddress;

DRAMAddress translate_address(uint32_t logical_address) {
    DRAMAddress dram_addr;
    dram_addr.bank = (logical_address >> (ROW_BITS + COL_BITS)) & ((1 << BANK_BITS) - 1);
    dram_addr.row = (logical_address >> COL_BITS) & ((1 << ROW_BITS) - 1);
    dram_addr.column = logical_address & ((1 << COL_BITS) - 1);
    
    return dram_addr;
}

void print_dram_address(DRAMAddress addr) {
    printf("Bank: %u, Row: %u, Column: %u\n", addr.bank, addr.row, addr.column);
}

int main() {
    uint32_t logical_address = 0x12345678;
    DRAMAddress dram_addr = translate_address(logical_address);
    
    printf("Logical Address: 0x%08X\n", logical_address);
    print_dram_address(dram_addr);
    
    return 0;
}
