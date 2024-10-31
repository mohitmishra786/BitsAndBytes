import numpy as np
import time

def matrix_multiply(A, B):
    n = len(A)
    C = [[0 for _ in range(n)] for _ in range(n)]
    
    start_time = time.time()
    
    # Basic triple loop multiplication
    for i in range(n):
        for j in range(n):
            for k in range(n):
                C[i][j] += A[i][k] * B[k][j]
                
    end_time = time.time()
    print(f"Time taken: {end_time - start_time:.4f} seconds")
    return C

# Generate random matrices
n = 1024
A = [[np.random.random() for _ in range(n)] for _ in range(n)]
B = [[np.random.random() for _ in range(n)] for _ in range(n)]

# Run multiplication
result = matrix_multiply(A, B)
