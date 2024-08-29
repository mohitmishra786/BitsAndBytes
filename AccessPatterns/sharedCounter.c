#include <stdatomic.h>
#include <threads.h>
#include <stdio.h>

#define NUM_THREADS 4
#define NUM_INCREMENTS 1000000

atomic_int counter = 0;

int increment_counter(void* arg) {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);
    }
    return 0;
}

int main() {
    thrd_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        thrd_create(&threads[i], increment_counter, NULL);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        thrd_join(threads[i], NULL);
    }

    printf("Final counter value: %d\n", atomic_load(&counter));
    return 0;
}
