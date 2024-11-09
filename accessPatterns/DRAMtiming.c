#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define NUM_BANKS 8
#define ROWS_PER_BANK 8192
#define COLUMNS_PER_ROW 1024
#define tCAS 14
#define tRAS 32
#define tRP 14
#define tRCD 14

typedef struct {
    int** data;
    int active_row;
    bool is_row_active;
    int last_activation_cycle;
} DRAMBank;

typedef struct {
    DRAMBank banks[NUM_BANKS];
    int current_cycle;
} DRAMChip;

int access_dram(DRAMChip* chip, int bank, int row, int column) {
    DRAMBank* current_bank = &chip->banks[bank];
    int cycles = 0;
    
    // Checking if we need to open a new row
    if (!current_bank->is_row_active || current_bank->active_row != row) {
        if (current_bank->is_row_active) {
            int cycles_since_activation = chip->current_cycle - current_bank->last_activation_cycle;
            if (cycles_since_activation < tRAS) {
                cycles += tRAS - cycles_since_activation;
            }
            cycles += tRP;  // Precharge timing
        }
        
        cycles += tRCD;
        current_bank->active_row = row;
        current_bank->is_row_active = true;
        current_bank->last_activation_cycle = chip->current_cycle + cycles;
    }
    
    cycles += tCAS;

    // Update current cycle
    chip->current_cycle += cycles;
    
    return cycles;
}

int main() {
    DRAMChip chip = {0};

    // Allocate memory for each bank's data array
    for (int i = 0; i < NUM_BANKS; i++) {
        chip.banks[i].data = (int**)malloc(ROWS_PER_BANK * sizeof(int*));
        if (chip.banks[i].data == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1); 
        }
        for (int j = 0; j < ROWS_PER_BANK; j++) {
            chip.banks[i].data[j] = (int*)malloc(COLUMNS_PER_ROW * sizeof(int));
            if (chip.banks[i].data[j] == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(1);
            }
        }
    }

    printf("Access (0,0,0): %d cycles\n", access_dram(&chip, 0, 0, 0));
    printf("Access (0,0,1): %d cycles\n", access_dram(&chip, 0, 0, 1));
    printf("Access (0,1,0): %d cycles\n", access_dram(&chip, 0, 1, 0));
    printf("Access (1,0,0): %d cycles\n", access_dram(&chip, 1, 0, 0));

    // Now let's free the dynamically allocated memory
    for (int i = 0; i < NUM_BANKS; i++) {
        for (int j = 0; j < ROWS_PER_BANK; j++) {
            free(chip.banks[i].data[j]);
        }
        free(chip.banks[i].data);
    }

    return 0;
}
