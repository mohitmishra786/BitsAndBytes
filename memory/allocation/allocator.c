#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define HDRP(bp) ((char *)(bp) - SIZE_T_SIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - 2*SIZE_T_SIZE)
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - SIZE_T_SIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - 2*SIZE_T_SIZE)))

#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
#define PACK(size, alloc) ((size) | (alloc))

#define GET_MARK(p) (GET(p) & 0x2)
#define SET_MARK(p) (PUT(p, GET(p) | 0x2))
#define RESET_MARK(p) (PUT(p, GET(p) & ~0x2))

static char *heap_start = NULL;
static char *heap_end = NULL;
static char *current_break = NULL;

static void *find_fit(size_t asize);
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *split_block(void *bp, size_t asize);

void m_init(size_t size) {
    heap_start = malloc(size);
    if (heap_start == NULL) {
        fprintf(stderr, "Failed to initialize heap\n");
        exit(1);
    }
    heap_end = heap_start + size;
    current_break = heap_start;

    PUT(HDRP(heap_start), PACK(size, 0));
    PUT(FTRP(heap_start), PACK(size, 0));
}

void *is_ptr(void *p) {
    if (p < (void *)heap_start || p >= (void *)heap_end)
        return NULL;

    char *block = heap_start;
    char *next_block;

    while (block < heap_end) {
        size_t size = GET_SIZE(HDRP(block));
        next_block = block + size;

        if (p >= (void *)block && p < (void *)next_block)
            return block;

        block = next_block;
    }

    return NULL;
}

static void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {
        return bp;
    }
    else if (prev_alloc && !next_alloc) {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    else if (!prev_alloc && next_alloc) {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    else {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}

static void *split_block(void *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) >= (2*SIZE_T_SIZE)) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize-asize, 0));
        PUT(FTRP(bp), PACK(csize-asize, 0));
    }
    else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
    return bp;
}

void *m_malloc(size_t size) {
    size_t asize;
    size_t extendsize;
    char *bp;

    if (size == 0)
        return NULL;

    if (size <= SIZE_T_SIZE)
        asize = 2*SIZE_T_SIZE;
    else
        asize = ALIGN(size + 2*SIZE_T_SIZE);

    // Search the free list for a fit
    if ((bp = find_fit(asize)) != NULL) {
        return split_block(bp, asize) + SIZE_T_SIZE;
    }

    // No fit found. Get more memory and place the block
    extendsize = (asize > 4096) ? asize : 4096;
    if ((bp = extend_heap(extendsize/SIZE_T_SIZE)) == NULL)
        return NULL;

    return split_block(bp, asize) + SIZE_T_SIZE;
}

void m_free(void *ptr) {
    if (ptr == NULL)
        return;

    char *bp = ptr - SIZE_T_SIZE;
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

static void mark(void *ptr) {
    void *block = is_ptr(ptr);
    if (block != NULL && !GET_MARK(HDRP(block))) {
        SET_MARK(HDRP(block));
        SET_MARK(FTRP(block));
        
        // Recursively mark all pointers in this block
        for (char *p = block; p < (char *)NEXT_BLKP(block); p += sizeof(void *)) {
            void *child = *(void **)p;
            if (child != NULL) {
                mark(child);
            }
        }
    }
}

static void sweep() {
    void *bp = heap_start;
    while (GET_SIZE(HDRP(bp)) > 0) {
        if (!GET_ALLOC(HDRP(bp)) || !GET_MARK(HDRP(bp))) {
            m_free(bp + SIZE_T_SIZE);
        }
        else {
            RESET_MARK(HDRP(bp));
            RESET_MARK(FTRP(bp));
        }
        bp = NEXT_BLKP(bp);
    }
}

void gc() {
    // Mark from roots (this is a simplified version, you'd need to implement mark_roots())
    // mark_roots();
    
    // Sweep
    sweep();
}

void print_heap() {
    void *bp = heap_start;
    while (GET_SIZE(HDRP(bp)) > 0) {
        printf("Block at %p: size = %u, allocated = %d\n", 
               bp, GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)));
        bp = NEXT_BLKP(bp);
    }
}

// Helper functions
static void *find_fit(size_t asize) {
    // First-fit search
    void *bp;

    for (bp = heap_start; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {
            return bp;
        }
    }
    return NULL; // No fit
}

static void *extend_heap(size_t words) {
    char *bp;
    size_t size;

    // Allocate an even number of words to maintain alignment
    size = (words % 2) ? (words+1) * SIZE_T_SIZE : words * SIZE_T_SIZE;
    if (current_break + size > heap_end)
        return NULL;  // Cannot extend heap

    bp = current_break;
    current_break += size;

    // Initialize free block header/footer and the epilogue header
    PUT(HDRP(bp), PACK(size, 0));         // Free block header
    PUT(FTRP(bp), PACK(size, 0));         // Free block footer

    return coalesce(bp);
}
