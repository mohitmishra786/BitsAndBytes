#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define NUM_BANKS 8
#define ROWS_PER_BANK 1024
#define COLS_PER_ROW 1024

#define tCAS 10
#define tRAS 28
#define tRP 10

typedef struct {
    int open_row;
    int cycle_opened;
} BankState;

BankState bank_states[NUM_BANKS] = {{-1, 0}};
int current_cycle = 0;

void memory_access(uint32_t address) {
    int bank = (address / COLS_PER_ROW) % NUM_BANKS;
    int row = (address / COLS_PER_ROW) / NUM_BANKS;
    int col = address % COLS_PER_ROW;

    printf("Accessing Bank: %d, Row: %d, Col: %d\n", bank, row, col);

    if (bank_states[bank].open_row != row) {
        if (bank_states[bank].open_row != -1) {
            // Need to close the current row first
            current_cycle += tRP;
            printf("Closing row %d, new cycle: %d\n", bank_states[bank].open_row, current_cycle);
        }
        // Open the new row
        current_cycle += tRAS;
        bank_states[bank].open_row = row;
        bank_states[bank].cycle_opened = current_cycle;
        printf("Opening row %d, new cycle: %d\n", row, current_cycle);
    }

    current_cycle += tCAS;
    printf("Accessing column, new cycle: %d\n", current_cycle);

    // We will check if we need to close the row (auto-precharge)
    if (current_cycle - bank_states[bank].cycle_opened >= tRAS) {
        current_cycle += tRP;
        bank_states[bank].open_row = -1;
        printf("Auto-precharge, new cycle: %d\n", current_cycle);
    }
}

int main() {
    memory_access(0);       // New row
    memory_access(1);       // Same row
    memory_access(COLS_PER_ROW * NUM_BANKS);  // New row, same bank
    memory_access(COLS_PER_ROW);  // New bank

    return 0;
}
