#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TAG_BITS 3
#define POINTER_MASK (~((1ULL << TAG_BITS) - 1))
#define TAG_MASK ((1ULL << TAG_BITS) - 1)

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_CUSTOM
} ObjectType;

typedef struct {
    ObjectType type;
    union {
        int int_value;
        float float_value;
        char* string_value;
        void* custom_value;
    } data;
} Object;

void* tag_pointer(void* ptr, uintptr_t tag) {
    uintptr_t addr = (uintptr_t)ptr;
    assert((addr & TAG_MASK) == 0 && "Pointer is already tagged or misaligned");
    return (void*)((addr & POINTER_MASK) | (tag & TAG_MASK));
}

void* untag_pointer(void* tagged_ptr) {
    return (void*)((uintptr_t)tagged_ptr & POINTER_MASK);
}

uintptr_t get_tag(void* tagged_ptr) {
    return (uintptr_t)tagged_ptr & TAG_MASK;
}

void* create_object(ObjectType type, void* value) {
    Object* obj = (Object*)malloc(sizeof(Object));
    if (!obj) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    obj->type = type;
    switch (type) {
        case TYPE_INT:
            obj->data.int_value = *(int*)value;
            break;
        case TYPE_FLOAT:
            obj->data.float_value = *(float*)value;
            break;
        case TYPE_STRING:
            obj->data.string_value = strdup((char*)value);
            if (!obj->data.string_value) {
                free(obj);
                printf("Memory allocation failed\n");
                return NULL;
            }
            break;
        case TYPE_CUSTOM:
            obj->data.custom_value = value;
            break;
    }
    return tag_pointer(obj, type);
}

void free_object(void* tagged_ptr) {
    Object* obj = untag_pointer(tagged_ptr);
    if (get_tag(tagged_ptr) == TYPE_STRING) {
        free(obj->data.string_value);
    }
    free(obj);
}

void print_object(void* tagged_ptr) {
    Object* obj = untag_pointer(tagged_ptr);
    switch (get_tag(tagged_ptr)) {
        case TYPE_INT:
            printf("Integer: %d\n", obj->data.int_value);
            break;
        case TYPE_FLOAT:
            printf("Float: %f\n", obj->data.float_value);
            break;
        case TYPE_STRING:
            printf("String: %s\n", obj->data.string_value);
            break;
        case TYPE_CUSTOM:
            printf("Custom object at address: %p\n", obj->data.custom_value);
            break;
    }
}

int main() {
    int int_value = 42;
    float float_value = 3.14f;
    char* string_value = "Hello, world!";
    int* custom_value = malloc(sizeof(int));
    if (!custom_value) {
        printf("Memory allocation failed\n");
        return 1;
    }
    *custom_value = 100;

    void* int_obj = create_object(TYPE_INT, &int_value);
    void* float_obj = create_object(TYPE_FLOAT, &float_value);
    void* string_obj = create_object(TYPE_STRING, string_value);
    void* custom_obj = create_object(TYPE_CUSTOM, custom_value);

    if (!int_obj || !float_obj || !string_obj || !custom_obj) {
        printf("Object creation failed\n");
        free(custom_value);
        return 1;
    }

    print_object(int_obj);
    print_object(float_obj);
    print_object(string_obj);
    print_object(custom_obj);

    free_object(int_obj);
    free_object(float_obj);
    free_object(string_obj);
    free_object(custom_obj);
    free(custom_value);

    return 0;
}
