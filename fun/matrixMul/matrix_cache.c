#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1024
#define BLOCK_SIZE 32  // Adjust based on your L1 cache size

// Function declarations
static inline int min(int a, int b) {
    return (a < b) ? a : b;
}

double** create_matrix() {
    double** matrix = (double**)malloc(N * sizeof(double*));
    if (matrix == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }
    
    for (int i = 0; i < N; i++) {
        matrix[i] = (double*)malloc(N * sizeof(double));
        if (matrix[i] == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            exit(1);
        }
        for (int j = 0; j < N; j++) {
            matrix[i][j] = (double)rand() / RAND_MAX;
        }
    }
    return matrix;
}

void free_matrix(double** matrix) {
    for (int i = 0; i < N; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void matrix_multiply_blocked(double** A, double** B, double** C) {
    clock_t start = clock();
    
    // Initialize C to zero
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
        }
    }
    
    // Blocked matrix multiplication
    for (int i = 0; i < N; i += BLOCK_SIZE) {
        for (int k = 0; k < N; k += BLOCK_SIZE) {
            for (int j = 0; j < N; j += BLOCK_SIZE) {
                // Multiply blocks
                for (int ii = i; ii < min(i + BLOCK_SIZE, N); ii++) {
                    for (int kk = k; kk < min(k + BLOCK_SIZE, N); kk++) {
                        for (int jj = j; jj < min(j + BLOCK_SIZE, N); jj++) {
                            C[ii][jj] += A[ii][kk] * B[kk][jj];
                        }
                    }
                }
            }
        }
    }
    
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time taken: %.4f seconds\n", time_spent);
}

int main() {
    srand(time(NULL));
    
    double** A = create_matrix();
    double** B = create_matrix();
    double** C = create_matrix();
    
    printf("Starting blocked matrix multiplication...\n");
    matrix_multiply_blocked(A, B, C);
    
    free_matrix(A);
    free_matrix(B);
    free_matrix(C);
    
    return 0;
}
