# Memory Alignment & Padding

## Basics of Memory Alignment

- **Memory Alignment**: The practice of aligning data in memory to start at addresses that are multiples of some power of two, optimizing access speed.

- **Padding**: Adding bytes to align data structures, which can lead to wasted space but improved performance.

## Implications

- **Performance**: Misaligned memory access can lead to performance penalties or even crashes on some architectures.
- **Struct Packing**: How compilers pack structures in memory to minimize padding or align for speed.

## Techniques for Alignment

- **Compiler Directives**: Using attributes or pragmas to control alignment.
  ```c
  struct __attribute__((aligned(16))) myStruct {
      int a;
      char b;
  };
Manual Padding: Adding extra fields to manually align data.

Books
"Computer Organization and Design" by David A. Patterson and John L. Hennessy
ISBN: 978-0128017333 (Discusses memory systems including alignment)

Online Resources
Memory Alignment: Detailed explanation on how and why memory alignment matters.
Memory Alignment (www.geeksforgeeks.org/memory-alignment-in-c-struct-members-alignment-padding/)

