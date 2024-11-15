#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Custom stack canary implementation
typedef struct {
    char buffer[20];
    unsigned int canary;
    void (*function_ptr)(void);
} protected_buffer;

// Generate random canary value
unsigned int generate_canary() {
    static int initialized = 0;
    if (!initialized) {
        srand(time(NULL));
        initialized = 1;
    }
    return rand() ^ 0xDEADBEEF;
}

// Verify canary value
int verify_canary(unsigned int canary, unsigned int expected) {
    if (canary != expected) {
        printf("[!] Stack corruption detected! Expected: 0x%08x, Found: 0x%08x\n",
               expected, canary);
        exit(1);
    }
    return 1;
}

void safe_function() {
    protected_buffer buf;
    unsigned int canary_value = generate_canary();
    
    // Initialize buffer and set canary
    memset(buf.buffer, 0, sizeof(buf.buffer));
    buf.canary = canary_value;
    buf.function_ptr = safe_function;
    
    // Get user input
    printf("Enter string: ");
    fgets(buf.buffer, sizeof(buf.buffer), stdin);
    
    // Verify canary before using function pointer
    verify_canary(buf.canary, canary_value);
    
    printf("Buffer contents: %s\n", buf.buffer);
}

int main() {
    safe_function();
    return 0;
}