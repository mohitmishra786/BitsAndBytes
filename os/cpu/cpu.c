#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 1024
#define NUM_REGISTERS 16
#define PIPELINE_STAGES 5

// Instruction Types
typedef enum {
    ADD = 0x00,
    SUB = 0x01,
    LOAD = 0x02,
    STORE = 0x03,
    JUMP = 0x04,
    BRANCH = 0x05
} OpCode;

// Instruction Structure
typedef struct {
    uint8_t opcode;
    uint8_t rd;
    uint8_t rs;
    uint8_t rt;
    uint16_t immediate;
} Instruction;

// Pipeline Stage Structure
typedef struct {
    uint32_t pc;
    Instruction inst;
    uint32_t result;
    int valid;
} PipelineStage;

// CPU State Structure
typedef struct {
    uint32_t programCounter;
    uint32_t registers[NUM_REGISTERS];
    uint8_t *memory;
    PipelineStage pipeline[PIPELINE_STAGES];
} CPUState;

// Function Prototypes
CPUState* initCPU();
void cleanupCPU(CPUState* cpu);
void pipelineCycle(CPUState* cpu);

// CPU Initialization
CPUState* initCPU() {
    CPUState* cpu = (CPUState*)malloc(sizeof(CPUState));
    if (!cpu) return NULL;
    
    cpu->programCounter = 0;
    memset(cpu->registers, 0, sizeof(uint32_t) * NUM_REGISTERS);
    
    cpu->memory = (uint8_t*)malloc(MEMORY_SIZE);
    if (!cpu->memory) {
        free(cpu);
        return NULL;
    }
    memset(cpu->memory, 0, MEMORY_SIZE);
    
    // Initialize pipeline
    memset(cpu->pipeline, 0, sizeof(PipelineStage) * PIPELINE_STAGES);
    
    return cpu;
}

void cleanupCPU(CPUState* cpu) {
    if (cpu) {
        free(cpu->memory);
        free(cpu);
    }
}

// Stage 1: Fetch
void fetchStage(CPUState* cpu) {
    if (cpu->programCounter >= MEMORY_SIZE - 3) return;
    
    // Read instruction bytes in correct order
    uint32_t instruction = (cpu->memory[cpu->programCounter] << 24) |
                          (cpu->memory[cpu->programCounter + 1] << 16) |
                          (cpu->memory[cpu->programCounter + 2] << 8) |
                          cpu->memory[cpu->programCounter + 3];
    
    // Update first pipeline stage
    cpu->pipeline[0].pc = cpu->programCounter;
    cpu->pipeline[0].inst.opcode = (instruction >> 24) & 0xFF;
    cpu->pipeline[0].inst.rd = (instruction >> 16) & 0xFF;
    cpu->pipeline[0].inst.rs = (instruction >> 8) & 0xFF;
    cpu->pipeline[0].inst.rt = instruction & 0xFF;
    cpu->pipeline[0].valid = 1;
    
    printf("Fetch: PC=%d, Instruction=0x%08x, Opcode=%d, RD=%d, RS=%d, RT=%d\n", 
           cpu->programCounter, instruction,
           cpu->pipeline[0].inst.opcode,
           cpu->pipeline[0].inst.rd,
           cpu->pipeline[0].inst.rs,
           cpu->pipeline[0].inst.rt);
           
    cpu->programCounter += 4;
}

// Stage 3: Execute
void executeStage(CPUState* cpu) {
    if (!cpu->pipeline[2].valid) return;
    
    Instruction* inst = &cpu->pipeline[2].inst;
    uint32_t result = 0;
    
    switch(inst->opcode) {
        case ADD:
            result = cpu->registers[inst->rs] + cpu->registers[inst->rt];
            printf("Execute: ADD R%d = R%d(%d) + R%d(%d) = %d\n",
                   inst->rd, inst->rs, cpu->registers[inst->rs],
                   inst->rt, cpu->registers[inst->rt], result);
            break;
        default:
            printf("Execute: Unknown opcode %d\n", inst->opcode);
            break;
    }
    
    cpu->pipeline[2].result = result;
}

// Stage 5: Writeback
void writebackStage(CPUState* cpu) {
    if (!cpu->pipeline[4].valid) return;
    
    Instruction* inst = &cpu->pipeline[4].inst;
    if (inst->opcode == ADD) {
        cpu->registers[inst->rd] = cpu->pipeline[4].result;
        printf("Writeback: R%d = %d\n", inst->rd, cpu->pipeline[4].result);
    }
}

void pipelineCycle(CPUState* cpu) {
    // Writeback (last stage)
    writebackStage(cpu);
    
    // Move instructions forward in pipeline
    for (int i = PIPELINE_STAGES - 1; i > 0; i--) {
        cpu->pipeline[i] = cpu->pipeline[i-1];
    }
    
    // Fetch new instruction (first stage)
    fetchStage(cpu);
    
    // Execute stage
    executeStage(cpu);
}

void printCPUState(CPUState* cpu) {
    printf("\nCPU State:\n");
    printf("Program Counter: %d\n", cpu->programCounter);
    printf("Register Values:\n");
    for(int i = 0; i < NUM_REGISTERS; i++) {
        printf("R%d = %d\n", i, cpu->registers[i]);
    }
    printf("\n");
}

int main() {
    CPUState* cpu = initCPU();
    if (!cpu) {
        printf("Failed to initialize CPU\n");
        return 1;
    }
    
    // Load test program: ADD R1, R2, R3
    // Instruction format: [opcode][rd][rs][rt]
    cpu->memory[0] = 0x00;  // ADD opcode (0x00)
    cpu->memory[1] = 0x01;  // R1 (destination)
    cpu->memory[2] = 0x02;  // R2 (source 1)
    cpu->memory[3] = 0x03;  // R3 (source 2)
    
    // Set initial register values
    cpu->registers[2] = 5;  // R2 = 5
    cpu->registers[3] = 3;  // R3 = 3
    
    printf("Initial state:\n");
    printCPUState(cpu);
    
    // Execute pipeline cycles
    for(int i = 0; i < PIPELINE_STAGES; i++) {
        printf("\nExecuting cycle %d:\n", i+1);
        pipelineCycle(cpu);
        printCPUState(cpu);
    }
    
    cleanupCPU(cpu);
    return 0;
}