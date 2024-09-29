# Assembly Language Programming

## Official Documentation
- **x86 and amd64 Instruction Reference**
  - [Link](https://www.felixcloutier.com/x86/)
- **ARM Developer: ARM Assembly Language**
  - [Link](https://developer.arm.com/documentation/dui0068/b/ARM-Instruction-Reference)
- **MIPS Architecture For Programmers**
  - [Link](https://www.mips.com/products/architectures/mips32-2/)

## Research Papers
- **"Using Assembly Language Programming to Teach Digital Systems Design" by G. Donzellini and D. Ponta (2013)**
  - [Link](https://ieeexplore.ieee.org/document/6684991)
- **"A Survey of Techniques for Improving Efficiency of Mobile Web Browsing" by A. Rajasekhar et al. (2020)**
  - [Link](https://ieeexplore.ieee.org/document/9108269)

## Additional Online Resources
- **OSDev.org: Assembly Language**
  - [Link](https://wiki.osdev.org/Assembly)
- **x86 Assembly Guide**
  - [Link](https://www.cs.virginia.edu/~evans/cs216/guides/x86.html)
- **NASM Tutorial**
  - [Link](https://cs.lmu.edu/~ray/notes/nasmtutorial/)

## Books
- **"Assembly Language for x86 Processors" by Kip R. Irvine**
  - ISBN: 0135381533
- **"Professional Assembly Language" by Richard Blum**
  - ISBN: 0764579010
- **"ARM Assembly Language Programming & Architecture" by Muhammad Ali Mazidi, Shujen Chen, Sarmad Naimi, and Sepehr Naimi**
  - ISBN: 0997925949

## For Deep Dive into Implementation
- **"The Art of Assembly Language" by Randall Hyde**
  - ISBN: 1593272073

## Standards
- **IEEE Std 694-1985 - IEEE Standard for Microprocessor Assembly Language**
  - Not freely available, but it provides guidelines for assembly language programming

## Code Example (x86 Assembly)
```nasm
section .data
    msg db 'Hello, World!', 0Ah    ; String to print
    len equ $ - msg                ; Length of the string

section .text
    global _start

_start:
    ; Write the string to stdout
    mov eax, 4          ; System call number for sys_write
    mov ebx, 1          ; File descriptor 1 is stdout
    mov ecx, msg        ; Address of string to write
    mov edx, len        ; Number of bytes to write
    int 80h             ; Call kernel

    ; Exit the program
    mov eax, 1          ; System call number for sys_exit
    xor ebx, ebx        ; Return 0 status
    int 80h             ; Call kernel
```

## Note
Assembly language programming provides deep insights into computer architecture and allows for fine-grained control over hardware. While not commonly used for large-scale application development, it's crucial for system programming, embedded systems, and performance-critical code sections. Understanding assembly also aids in debugging and reverse engineering.
