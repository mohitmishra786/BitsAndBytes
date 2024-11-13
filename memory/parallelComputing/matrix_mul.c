#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define MATRIX_SIZE 1024
#define BLOCK_SIZE 32

void tensor_core_simulation(float* A, float* B, float* C, float* D, int M, int N, int K) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < M; i += BLOCK_SIZE) {
        for (int j = 0; j < N; j += BLOCK_SIZE) {
            float temp[BLOCK_SIZE][BLOCK_SIZE] = {0};
            
            // Matrix multiplication in blocks
            for (int k = 0; k < K; k += BLOCK_SIZE) {
                for (int bi = 0; bi < BLOCK_SIZE; bi++) {
                    for (int bj = 0; bj < BLOCK_SIZE; bj++) {
                        float sum = 0.0f;
                        for (int bk = 0; bk < BLOCK_SIZE; bk++) {
                            sum += A[(i+bi)*K + (k+bk)] * 
                                  B[(k+bk)*N + (j+bj)];
                        }
                        temp[bi][bj] += sum;
                    }
                }
            }
            
            // Add bias and store result
            for (int bi = 0; bi < BLOCK_SIZE; bi++) {
                for (int bj = 0; bj < BLOCK_SIZE; bj++) {
                    D[(i+bi)*N + (j+bj)] = 
                        temp[bi][bj] + C[(i+bi)*N + (j+bj)];
                }
            }
        }
}
    }


int main() {
    float *A, *B, *C, *D;
    
    // Allocate matrices
    A = (float*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(float));
    B = (float*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(float));
    C = (float*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(float));
    D = (float*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(float));
    
    // Initialize matrices with random values
    srand(time(NULL));
    for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        A[i] = (float)rand() / RAND_MAX;
        B[i] = (float)rand() / RAND_MAX;
        C[i] = (float)rand() / RAND_MAX;
    }
    
    // Measure execution time
    double start_time = omp_get_wtime();
    tensor_core_simulation(A, B, C, D, MATRIX_SIZE, MATRIX_SIZE, MATRIX_SIZE);
    double end_time = omp_get_wtime();
    
    printf("Matrix multiplication completed in %.3f seconds\n", 
           end_time - start_time);
    
    // Print sample results
    printf("Sample output (top-left 2x2 corner):\n");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            printf("%.4f ", D[i * MATRIX_SIZE + j]);
        }
        printf("\n");
    }
    
    free(A);
    free(B);
    free(C);
    free(D);
    return 0;
}