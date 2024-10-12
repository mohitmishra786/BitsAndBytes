#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdbool.h>

#define DEFINE_UNIQUE_PTR(T) \
typedef struct { \
    T* ptr; \
    void (*deleter)(T*); \
    _Atomic bool is_owned; \
} unique_ptr_##T; \
\
unique_ptr_##T unique_ptr_create_##T(T* p, void (*d)(T*)) { \
    unique_ptr_##T up; \
    up.ptr = p; \
    up.deleter = d ? d : (void (*)(T*))free; \
    atomic_init(&up.is_owned, true); \
    return up; \
} \
\
void unique_ptr_destroy_##T(unique_ptr_##T* up) { \
    if (atomic_exchange(&up->is_owned, false)) { \
        if (up->ptr) { \
            up->deleter(up->ptr); \
            up->ptr = NULL; \
        } \
    } \
} \
\
T* unique_ptr_get_##T(unique_ptr_##T* up) { \
    return up->ptr; \
} \
\
T* unique_ptr_release_##T(unique_ptr_##T* up) { \
    T* tmp = up->ptr; \
    if (atomic_exchange(&up->is_owned, false)) { \
        up->ptr = NULL; \
    } else { \
        tmp = NULL; \
    } \
    return tmp; \
} \
\
void unique_ptr_reset_##T(unique_ptr_##T* up, T* p) { \
    if (atomic_load(&up->is_owned)) { \
        T* old = atomic_exchange(&up->ptr, p); \
        if (old) { \
            up->deleter(old); \
        } \
    } \
} \
\
unique_ptr_##T unique_ptr_move_##T(unique_ptr_##T* up) { \
    unique_ptr_##T new_up = *up; \
    if (atomic_exchange(&up->is_owned, false)) { \
        up->ptr = NULL; \
    } else { \
        new_up.ptr = NULL; \
        atomic_store(&new_up.is_owned, false); \
    } \
    return new_up; \
}

DEFINE_UNIQUE_PTR(int)

void custom_int_deleter(int* ptr) {
    printf("Custom deleter called for int: %d\n", *ptr);
    free(ptr);
}

int main() {
    int* raw_ptr = malloc(sizeof(int));
    *raw_ptr = 42;
    
    unique_ptr_int up1 = unique_ptr_create_int(raw_ptr, custom_int_deleter);
    printf("up1 value: %d\n", *unique_ptr_get_int(&up1));
    
    unique_ptr_int up2 = unique_ptr_move_int(&up1);
    printf("up2 value: %d\n", *unique_ptr_get_int(&up2));
    
    if (unique_ptr_get_int(&up1) == NULL) {
        printf("up1 is now empty\n");
    }
    
    unique_ptr_destroy_int(&up1);
    unique_ptr_destroy_int(&up2);
    
    return 0;
}
