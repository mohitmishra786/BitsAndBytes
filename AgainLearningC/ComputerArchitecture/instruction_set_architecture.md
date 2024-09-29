# Instruction Set Architecture (ISA)

## Official Documentation
- **ARM Architecture Reference Manual**
  - [Link](https://developer.arm.com/documentation/ddi0487/latest)
- **Intel® 64 and IA-32 Architectures Software Developer's Manual**
  - [Link](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- **RISC-V Specifications**
  - [Link](https://riscv.org/technical/specifications/)

## Research Papers
- **"A Survey of Instruction Set Architecture Heterogeneity in University Computer Architecture Courses" by D. Franklin et al. (2019)**
  - [Link](https://ieeexplore.ieee.org/document/8812897)
- **"A Survey of Techniques for Architectural Support of Data Analytics" by M. Ahmadi et al. (2021)**
  - [Link](https://dl.acm.org/doi/10.1145/3456727)

## Additional Online Resources
- **Computerphile: Instruction Set Architectures**
  - [Link](https://www.youtube.com/watch?v=vqa9TVketPY)
- **MIT OpenCourseWare: 6.004 Computation Structures**
  - [Link](https://ocw.mit.edu/courses/6-004-computation-structures-spring-2017/)
- **Stanford CS107: Computer Organization & Systems**
  - [Link](https://web.stanford.edu/class/cs107/)

## Books
- **"Computer Organization and Design RISC-V Edition" by David A. Patterson and John L. Hennessy**
  - ISBN: 0128203315
- **"Computer Architecture: A Quantitative Approach" by John L. Hennessy and David A. Patterson**
  - ISBN: 0128119055
- **"Structured Computer Organization" by Andrew S. Tanenbaum and Todd Austin**
  - ISBN: 0134997190

## For Deep Dive into Implementation
- **"Modern Processor Design: Fundamentals of Superscalar Processors" by John Paul Shen and Mikko H. Lipasti**
  - ISBN: 1478627425

## Standards
- **IEEE 1754-1994 - IEEE Standard for a 32-Bit Microprocessor Architecture**
  - Not freely available, but it's an example of a standardized ISA

## Code Example (RISC-V Assembly)
```assembly
# Simple RISC-V assembly program to add two numbers
.global _start

.text
_start:
    li a0, 10       # Load immediate value 10 into register a0
    li a1, 20       # Load immediate value 20 into register a1
    add a2, a0, a1  # Add a0 and a1, store result in a2

    # Exit program
    li a7, 93       # System call number for exit
    ecall           # Make system call
```

## Note
Understanding ISA is crucial for both hardware designers and software developers. It defines the interface between hardware and software, influencing aspects such as performance, power efficiency, and programming complexity. Different ISAs (like RISC and CISC) have their own design philosophies and trade-offs.
