#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

typedef struct {
    uint8_t age;
    uint32_t id;
    uint16_t year;
} Person;

void print_memory_layout(void* ptr, size_t size) {
    unsigned char* bytes = (unsigned char*)ptr;
    printf("Memory layout (hex): ");
    for (size_t i = 0; i < size; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");
}

int main() {
    Person person = {25, 12345678, 2024};

    printf("Size of Person struct: %zu bytes\n", sizeof(Person));
    printf("Offset of age: %zu\n", offsetof(Person, age));
    printf("Offset of id: %zu\n", offsetof(Person, id));
    printf("Offset of year: %zu\n", offsetof(Person, year));
    
    print_memory_layout(&person, sizeof(Person));
    return 0;
}
