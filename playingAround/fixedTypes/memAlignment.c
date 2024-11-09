#include <stdio.h>
#include <stdint.h>

// Poorly aligned structure
typedef struct {
    char a;      // 1 byte
    int32_t b;   // 4 bytes
    char c;      // 1 byte
} BadStruct;     // Actual size: 12 bytes due to padding

// Well aligned structure
typedef struct {
    int32_t b;   // 4 bytes
    char a;      // 1 byte
    char c;      // 1 byte
    // 2 bytes padding
} GoodStruct;    // Actual size: 8 bytes

int main() {
    printf("Size of BadStruct: %zu\n", sizeof(BadStruct));
    printf("Size of GoodStruct: %zu\n", sizeof(GoodStruct));
    return 0;
}
