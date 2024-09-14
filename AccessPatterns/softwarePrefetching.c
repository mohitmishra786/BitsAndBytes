#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <x86intrin.h>

#define ARRAY_SIZE 100000000
#define CACHE_LINE 64
#define PREFETCH_DISTANCE 10

typedef struct {
    int value;
    char padding[60];  // Ensure each element is in a separate cache line
} PaddedInt;

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

// Simple array traversal without prefetching
void traverse_no_prefetch(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i]++;  // Simple operation to ensure the compiler doesn't optimize away the loop
    }
}

// Array traversal with software prefetching
void traverse_with_prefetch(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        _mm_prefetch((char*)&arr[i + PREFETCH_DISTANCE], _MM_HINT_T0);
        arr[i]++;
    }
}

// Strided access pattern with prefetching
void strided_access_prefetch(int* arr, int size, int stride) {
    for (int i = 0; i < size; i += stride) {
        _mm_prefetch((char*)&arr[i + stride * PREFETCH_DISTANCE], _MM_HINT_T0);
        arr[i]++;
    }
}

// Linked list traversal with prefetching
typedef struct Node {
    int value;
    struct Node* next;
} Node;

void linked_list_prefetch(Node* head) {
    Node* current = head;
    while (current != NULL) {
        if (current->next != NULL) {
            _mm_prefetch((char*)current->next, _MM_HINT_T0);
            if (current->next->next != NULL) {
                _mm_prefetch((char*)current->next->next, _MM_HINT_T0);
            }
        }
        current->value++;
        current = current->next;
    }
}

// Binary tree traversal with prefetching
typedef struct TreeNode {
    int value;
    struct TreeNode *left, *right;
} TreeNode;

void tree_prefetch(TreeNode* root) {
    if (root == NULL) return;

    if (root->left) _mm_prefetch((char*)root->left, _MM_HINT_T0);
    if (root->right) _mm_prefetch((char*)root->right, _MM_HINT_T0);

    root->value++;
    tree_prefetch(root->left);
    tree_prefetch(root->right);
}

Node* create_linked_list(int size) {
    Node* head = NULL;
    Node* current = NULL;
    for (int i = 0; i < size; i++) {
        Node* new_node = (Node*)malloc(sizeof(Node));
        new_node->value = i;
        new_node->next = NULL;
        if (head == NULL) {
            head = new_node;
            current = head;
        } else {
            current->next = new_node;
            current = new_node;
        }
    }
    return head;
}

TreeNode* create_binary_tree(int depth) {
    if (depth == 0) return NULL;
    TreeNode* root = (TreeNode*)malloc(sizeof(TreeNode));
    root->value = rand() % 100;
    root->left = create_binary_tree(depth - 1);
    root->right = create_binary_tree(depth - 1);
    return root;
}

void free_binary_tree(TreeNode* root) {
    if (root == NULL) return;
    free_binary_tree(root->left);
    free_binary_tree(root->right);
    free(root);
}

// Complex data structure with pointer chasing
void complex_structure_prefetch(PaddedInt** matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            _mm_prefetch((char*)&matrix[i][j + PREFETCH_DISTANCE], _MM_HINT_T0);
            matrix[i][j].value++;
        }
    }
}

int main() {
    srand(time(NULL));
    
    double start, end;

    // Test simple array traversal
    int* arr = (int*)aligned_alloc(CACHE_LINE, ARRAY_SIZE * sizeof(int));
    for (int i = 0; i < ARRAY_SIZE; i++) arr[i] = i;

    start = get_time();
    traverse_no_prefetch(arr, ARRAY_SIZE);
    end = get_time();
    printf("No prefetch time: %f seconds\n", end - start);

    start = get_time();
    traverse_with_prefetch(arr, ARRAY_SIZE);
    end = get_time();
    printf("With prefetch time: %f seconds\n", end - start);

    // Test strided access
    start = get_time();
    strided_access_prefetch(arr, ARRAY_SIZE, 16);
    end = get_time();
    printf("Strided access with prefetch time: %f seconds\n", end - start);

    free(arr);

    // Test linked list traversal
    Node* list = create_linked_list(1000000);
    start = get_time();
    linked_list_prefetch(list);
    end = get_time();
    printf("Linked list with prefetch time: %f seconds\n", end - start);

    // Free linked list
    Node* current = list;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }

    // Test binary tree traversal
    TreeNode* tree = create_binary_tree(20);
    start = get_time();
    tree_prefetch(tree);
    end = get_time();
    printf("Binary tree with prefetch time: %f seconds\n", end - start);

    // Free binary tree
    free_binary_tree(tree);

    // Test complex structure prefetching
    int rows = 1000, cols = 1000;
    PaddedInt** matrix = (PaddedInt**)malloc(rows * sizeof(PaddedInt*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (PaddedInt*)aligned_alloc(CACHE_LINE, cols * sizeof(PaddedInt));
    }

    start = get_time();
    complex_structure_prefetch(matrix, rows, cols);
    end = get_time();
    printf("Complex structure with prefetch time: %f seconds\n", end - start);

    // Free matrix
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}
