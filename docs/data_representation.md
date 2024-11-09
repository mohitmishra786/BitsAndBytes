# Data Representation

## Official Documentation
- **IEEE 754-2019 - IEEE Standard for Floating-Point Arithmetic**
  - [Link](https://ieeexplore.ieee.org/document/8766229)
- **The Unicode® Standard Version 15.0**
  - [Link](https://www.unicode.org/versions/Unicode15.0.0/)

## Research Papers
- **"A Survey of Techniques for Dynamic Branch Prediction" by S. Mittal (2019)**
  - [Link](https://ieeexplore.ieee.org/document/8444667)
- **"A Survey of Architectural Techniques for Near-Threshold Computing" by U. R. Karpuzcu et al. (2015)**
  - [Link](https://dl.acm.org/doi/10.1145/2677091)

## Additional Online Resources
- **Computerphile: Floating Point Numbers**
  - [Link](https://www.youtube.com/watch?v=PZRI1IfStY0)
- **GeeksforGeeks: Data Types in C**
  - [Link](https://www.geeksforgeeks.org/data-types-in-c/)
- **IBM Knowledge Center: Character Data Representation**
  - [Link](https://www.ibm.com/docs/en/i/7.4?topic=concepts-character-data-representation)

## Books
- **"Structured Computer Organization" by Andrew S. Tanenbaum and Todd Austin**
  - ISBN: 0132916525
- **"Computer Organization and Architecture: Designing for Performance" by William Stallings**
  - ISBN: 0134101618
- **"Digital Design and Computer Architecture: ARM Edition" by Sarah Harris and David Harris**
  - ISBN: 0128000562

## For Deep Dive into Implementation
- **"Computer Architecture: A Quantitative Approach" by John L. Hennessy and David A. Patterson**
  - ISBN: 0128119055

## Standards
- **ISO/IEC 10646:2020 Information technology — Universal Coded Character Set (UCS)**
  - Not freely available, but it's the international standard for character encoding

## Code Example
```c
#include <stdio.h>
#include <limits.h>
#include <float.h>

int main() {
    printf("Signed int: %d to %d\n", INT_MIN, INT_MAX);
    printf("Unsigned int: 0 to %u\n", UINT_MAX);
    printf("Float: %e to %e\n", FLT_MIN, FLT_MAX);
    
    char c = 'A';
    printf("Character '%c' has ASCII value %d\n", c, c);
    
    return 0;
}
```

## Note
Understanding data representation is crucial for efficient programming and optimizing memory usage. It's important to be aware of the limitations and precision of different data types, especially when working with floating-point arithmetic or character encoding in international contexts.
