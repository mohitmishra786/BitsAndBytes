#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <immintrin.h>
#include <stdint.h>  

#define N 4096
#define BLOCK_SIZE 64
#define ALIGN_TO 32

static inline int min(int a, int b) {
    return (a < b) ? a : b;
}

void initialize_matrix(double* matrix) {
    if (!matrix) return;
    
    #pragma omp parallel for
    for (int i = 0; i < N * N; i++) {
        matrix[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
    }
}

double* create_aligned_matrix() {
    double* matrix = NULL;
    size_t size = (size_t)N * N * sizeof(double);
    if (size / sizeof(double) != N * N) {
        fprintf(stderr, "Matrix size overflow!\n");
        return NULL;
    }
    
    if (posix_memalign((void**)&matrix, ALIGN_TO, size) != 0 || !matrix) {
        fprintf(stderr, "Memory allocation failed for matrix of size %zu!\n", size);
        return NULL;
    }
    
    initialize_matrix(matrix);
    return matrix;
}

void validate_result(const double* A, const double* B, const double* C) {
    if (!A || !B || !C) return;
    
    int errors = 0;
    int validate_size = min(5, N);
    #pragma omp parallel for reduction(+:errors)
    for (int i = 0; i < validate_size; i++) {
        for (int j = 0; j < validate_size; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += A[i * N + k] * B[k * N + j];
            }
            if (fabs(sum - C[i * N + j]) > 1e-5) {
                #pragma omp critical
                {
                    printf("Validation failed at [%d,%d]: Expected %f, Got %f\n",
                           i, j, sum, C[i * N + j]);
                    errors++;
                }
            }
        }
    }
    if (errors == 0) {
        printf("Basic validation passed!\n");
    }
}

void matrix_multiply_optimized(const double* A, const double* B, double* C) {
    if (!A || !B || !C) return;
    
    double start_time = omp_get_wtime();
    
    memset(C, 0, N * N * sizeof(double));
    
    #pragma omp parallel for collapse(2) schedule(dynamic, 1)
    for (int i = 0; i < N; i += BLOCK_SIZE) {
        for (int j = 0; j < N; j += BLOCK_SIZE) {
            for (int k = 0; k < N; k += BLOCK_SIZE) {
                for (int ii = i; ii < min(i + BLOCK_SIZE, N); ii++) {
                    for (int kk = k; kk < min(k + BLOCK_SIZE, N); kk++) {
                        __m256d a = _mm256_set1_pd(A[ii * N + kk]);
                        
                        for (int jj = j; jj < min(j + BLOCK_SIZE, N); jj += 4) {
                            __m256d b = _mm256_loadu_pd(&B[kk * N + jj]);
                            __m256d c = _mm256_loadu_pd(&C[ii * N + jj]);
                            c = _mm256_fmadd_pd(a, b, c);
                            _mm256_storeu_pd(&C[ii * N + jj], c);
                        }
                    }
                }
            }
        }
    }
    
    double end_time = omp_get_wtime();
    printf("Time taken: %.4f seconds\n", end_time - start_time);
}

void print_system_info() {
    printf("Matrix multiplication configuration:\n");
    printf("Matrix size: %d x %d\n", N, N);
    printf("Block size: %d\n", BLOCK_SIZE);
    printf("Number of OpenMP threads: %d\n", omp_get_max_threads());
    
    FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
    if (cpuinfo) {
        char line[256];
        while (fgets(line, sizeof(line), cpuinfo)) {
            if (strncmp(line, "model name", 10) == 0) {
                printf("CPU: %s", line + 13);
                break;
            }
        }
        fclose(cpuinfo);
    }
    
    FILE *meminfo = fopen("/proc/meminfo", "r");
    if (meminfo) {
        char line[256];
        while (fgets(line, sizeof(line), meminfo)) {
            if (strncmp(line, "MemTotal:", 9) == 0) {
                printf("Total Memory: %s", line + 10);
                break;
            }
        }
        fclose(meminfo);
    }
    printf("\n");
}

int main() {
    srand(1234);
    
    print_system_info();
    
    printf("\nAllocating and initializing matrices...\n");
    double* A = create_aligned_matrix();
    double* B = create_aligned_matrix();
    double* C = create_aligned_matrix();
    
    if (!A || !B || !C) {
        fprintf(stderr, "Memory allocation failed!\n");
        free(A);
        free(B);
        free(C);
        return 1;
    }
    
    omp_set_num_threads(omp_get_num_procs());
    
    printf("\nStarting optimized matrix multiplication...\n");
    matrix_multiply_optimized(A, B, C);
    
    printf("\nValidating results...\n");
    validate_result(A, B, C);
    
    free(A);
    free(B);
    free(C);
    
    return 0;
}
