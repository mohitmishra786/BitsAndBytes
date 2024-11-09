#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1024

double** create_matrix() {
    double** matrix = (double**)malloc(N * sizeof(double*));
    for (int i = 0; i < N; i++) {
        matrix[i] = (double*)malloc(N * sizeof(double));
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

void matrix_multiply(double** A, double** B, double** C) {
    clock_t start = clock();
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
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
    
    printf("Starting basic matrix multiplication...\n");
    matrix_multiply(A, B, C);
    
    free_matrix(A);
    free_matrix(B);
    free_matrix(C);
    
    return 0;
}
