#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>

#define DEFINE_SHARED_PTR(T) \
typedef struct { \
    T* data; \
    _Atomic size_t* ref_count; \
    void (*deleter)(T*); \
    pthread_mutex_t mutex; \
} shared_ptr_##T; \
\
shared_ptr_##T* create_shared_##T(T value, void (*custom_deleter)(T*)) { \
    shared_ptr_##T* ptr = (shared_ptr_##T*)malloc(sizeof(shared_ptr_##T)); \
    if (!ptr) return NULL; \
    \
    ptr->data = (T*)malloc(sizeof(T)); \
    if (!ptr->data) { \
        free(ptr); \
        return NULL; \
    } \
    *ptr->data = value; \
    \
    ptr->ref_count = (_Atomic size_t*)malloc(sizeof(_Atomic size_t)); \
    if (!ptr->ref_count) { \
        free(ptr->data); \
        free(ptr); \
        return NULL; \
    } \
    atomic_init(ptr->ref_count, 1); \
    \
    ptr->deleter = custom_deleter ? custom_deleter : (void (*)(T*))free; \
    \
    if (pthread_mutex_init(&ptr->mutex, NULL) != 0) { \
        free(ptr->ref_count); \
        free(ptr->data); \
        free(ptr); \
        return NULL; \
    } \
    \
    return ptr; \
} \
\
void shared_ptr_inc_ref_##T(shared_ptr_##T* ptr) { \
    if (ptr && ptr->ref_count) { \
        pthread_mutex_lock(&ptr->mutex); \
        atomic_fetch_add(ptr->ref_count, 1); \
        pthread_mutex_unlock(&ptr->mutex); \
    } \
} \
\
void shared_ptr_dec_ref_##T(shared_ptr_##T* ptr) { \
    if (ptr && ptr->ref_count) { \
        pthread_mutex_lock(&ptr->mutex); \
        if (atomic_fetch_sub(ptr->ref_count, 1) == 1) { \
            pthread_mutex_unlock(&ptr->mutex); \
            ptr->deleter(ptr->data); \
            free(ptr->ref_count); \
            pthread_mutex_destroy(&ptr->mutex); \
            free(ptr); \
        } else { \
            pthread_mutex_unlock(&ptr->mutex); \
        } \
    } \
} \
\
T* shared_ptr_get_##T(shared_ptr_##T* ptr) { \
    return ptr ? ptr->data : NULL; \
}

DEFINE_SHARED_PTR(int)

void custom_int_deleter(int* ptr) {
    printf("Custom deleter called for int: %d\n", *ptr);
    free(ptr);
}

int main() {
    shared_ptr_int* ptr1 = create_shared_int(42, custom_int_deleter);
    shared_ptr_int* ptr2 = create_shared_int(10, NULL);

    printf("ptr1 value: %d\n", *shared_ptr_get_int(ptr1));
    printf("ptr2 value: %d\n", *shared_ptr_get_int(ptr2));

    shared_ptr_inc_ref_int(ptr1);
    shared_ptr_dec_ref_int(ptr1);
    shared_ptr_dec_ref_int(ptr1);
    shared_ptr_dec_ref_int(ptr2);

    return 0;
}
