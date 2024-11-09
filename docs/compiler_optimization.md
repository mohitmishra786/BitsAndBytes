# Compiler Optimization

## Introduction to Compiler Optimization

- **Definition**: Techniques used by compilers to improve the performance or efficiency of the executable code.

## Common Optimization Techniques

- **Loop Unrolling**: Increases the program's speed by reducing loop overhead.
- **Dead Code Elimination**: Removes code that does not affect the program results.
- **Register Allocation**: Efficient use of registers to minimize memory access.

## Architecture-Specific Optimizations

- **Vectorization**: Utilizing SIMD instructions (like SSE, AVX on x86) for parallel operations.
- **Instruction Scheduling**: Reordering instructions to reduce latency and improve pipeline efficiency.

## Books

- "Compilers: Principles, Techniques, and Tools" by Alfred V. Aho, Monica S. Lam, Ravi Sethi, and Jeffrey D. Ullman
  - ISBN: 978-0321486813

## Tools

- **GCC**: GNU Compiler Collection with various optimization flags.
- **Clang/LLVM**: Provides detailed diagnostics and optimization capabilities.

## Online Resources

- **Compiler Explorer**: An interactive compiler allowing to see how code is optimized.
  - [Godbolt's Compiler Explorer](godbolt.org)
