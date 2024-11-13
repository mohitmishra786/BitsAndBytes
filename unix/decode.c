#include <stdio.h>
#include <stdint.h>

typedef struct {
    uint8_t has_rex;
    uint8_t rex_w;
    uint8_t rex_r;
    uint8_t rex_x;
    uint8_t rex_b;
    uint8_t modrm;
    uint8_t sib;
    uint8_t opcode;
} x86_instruction;

void decode_instruction(uint8_t *bytes, size_t length, x86_instruction *inst) {
    size_t index = 0;
    
    // Check for REX prefix
    if ((bytes[index] & 0xF0) == 0x40) {
        inst->has_rex = 1;
        inst->rex_w = (bytes[index] & 0x08) >> 3;
        inst->rex_r = (bytes[index] & 0x04) >> 2;
        inst->rex_x = (bytes[index] & 0x02) >> 1;
        inst->rex_b = bytes[index] & 0x01;
        index++;
    }
    
    // Get opcode
    inst->opcode = bytes[index++];
    
    // Check if instruction has ModRM byte
    if (index < length) {
        inst->modrm = bytes[index++];
        
        // Check if SIB byte is present
        uint8_t mod = (inst->modrm >> 6) & 0x03;
        uint8_t rm = inst->modrm & 0x07;
        if (mod != 0x03 && rm == 0x04) {
            inst->sib = bytes[index++];
        }
    }
}

int main() {
    // Example instruction bytes (MOV RAX, RCX with REX prefix)
    uint8_t instruction[] = { 0x48, 0x89, 0xC8 };
    x86_instruction decoded_inst = {0};
    
    decode_instruction(instruction, sizeof(instruction), &decoded_inst);
    
    printf("REX.W: %d\n", decoded_inst.rex_w);
    printf("REX.R: %d\n", decoded_inst.rex_r);
    printf("REX.X: %d\n", decoded_inst.rex_x);
    printf("REX.B: %d\n", decoded_inst.rex_b);
    printf("Opcode: 0x%02X\n", decoded_inst.opcode);
    printf("ModRM: 0x%02X\n", decoded_inst.modrm);
    
    return 0;
}
