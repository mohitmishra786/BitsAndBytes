#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_BANKS 4
#define QUEUE_SIZE 16

typedef enum {
    CMD_ACTIVATE,
    CMD_READ,
    CMD_WRITE,
    CMD_PRECHARGE
} CommandType;

typedef struct {
    CommandType type;
    uint32_t bank;
    uint32_t row;
    uint32_t column;
} Command;

typedef struct {
    Command queue[QUEUE_SIZE];
    int front;
    int rear;
    int size;
} CommandQueue;

typedef struct {
    bool is_open;
    uint32_t open_row;
} BankState;

CommandQueue cmd_queue;
BankState bank_states[NUM_BANKS];

void init_command_queue() {
    cmd_queue.front = 0;
    cmd_queue.rear = -1;
    cmd_queue.size = 0;
}

bool is_queue_full() {
    return cmd_queue.size == QUEUE_SIZE;
}

bool is_queue_empty() {
    return cmd_queue.size == 0;
}

void enqueue_command(Command cmd) {
    if (is_queue_full()) {
        printf("Error: Command queue is full\n");
        return;
    }
    
    cmd_queue.rear = (cmd_queue.rear + 1) % QUEUE_SIZE;
    cmd_queue.queue[cmd_queue.rear] = cmd;
    cmd_queue.size++;
}

Command dequeue_command() {
    if (is_queue_empty()) {
        printf("Error: Command queue is empty\n");
        exit(1);
    }
    
    Command cmd = cmd_queue.queue[cmd_queue.front];
    cmd_queue.front = (cmd_queue.front + 1) % QUEUE_SIZE;
    cmd_queue.size--;
    
    return cmd;
}

void schedule_commands() {
    while (!is_queue_empty()) {
        Command cmd = dequeue_command();
        
        switch (cmd.type) {
            case CMD_ACTIVATE:
                if (!bank_states[cmd.bank].is_open) {
                    bank_states[cmd.bank].is_open = true;
                    bank_states[cmd.bank].open_row = cmd.row;
                    printf("Activate Bank %u, Row %u\n", cmd.bank, cmd.row);
                }
                break;
            
            case CMD_READ:
                if (bank_states[cmd.bank].is_open && bank_states[cmd.bank].open_row == cmd.row) {
                    printf("Read Bank %u, Row %u, Column %u\n", cmd.bank, cmd.row, cmd.column);
                } else {
                    Command activate_cmd = {CMD_ACTIVATE, cmd.bank, cmd.row, 0};
                    enqueue_command(activate_cmd);
                    enqueue_command(cmd);
                }
                break;
            
            case CMD_WRITE:
                if (bank_states[cmd.bank].is_open && bank_states[cmd.bank].open_row == cmd.row) {
                    printf("Write Bank %u, Row %u, Column %u\n", cmd.bank, cmd.row, cmd.column);
                } else {
                    Command activate_cmd = {CMD_ACTIVATE, cmd.bank, cmd.row, 0};
                    enqueue_command(activate_cmd);
                    enqueue_command(cmd);
                }
                break;
            
            case CMD_PRECHARGE:
                if (bank_states[cmd.bank].is_open) {
                    bank_states[cmd.bank].is_open = false;
                    printf("Precharge Bank %u\n", cmd.bank);
                }
                break;
        }
    }
}

int main() {
    init_command_queue();
    
    for (int i = 0; i < NUM_BANKS; i++) {
        bank_states[i].is_open = false;
    }
    
    Command cmd1 = {CMD_ACTIVATE, 0, 1000, 0};
    Command cmd2 = {CMD_READ, 0, 1000, 50};
    Command cmd3 = {CMD_WRITE, 1, 2000, 100};
    Command cmd4 = {CMD_READ, 0, 1000, 60};
    Command cmd5 = {CMD_PRECHARGE, 0, 0, 0};
    
    enqueue_command(cmd1);
    enqueue_command(cmd2);
    enqueue_command(cmd3);
    enqueue_command(cmd4);
    enqueue_command(cmd5);
    
    schedule_commands();
    
    return 0;
}
