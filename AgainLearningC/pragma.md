# Pragma Directives and Struct Padding

## Official Documentation

- **GCC Documentation on Structure-Packing Pragmas**
  - [Link](https://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Structure_002dPacking-Pragmas.html)
- **Microsoft Docs on pack pragma**
  - [Link](https://learn.microsoft.com/en-us/cpp/preprocessor/pack?view=msvc-170)
- **Clang Documentation on #pragma pack**
  - [Link](https://clang.llvm.org/docs/UsersManual.html#controlling-structure-layout)
- **ARM Documentation on #pragma pack**
  - [Link](https://developer.arm.com/documentation/dui0491/i/Compiler-specific-Features/-pragma-pack-n-)

## Research Papers

- **"An Empirical Study on the Use of Directives in Parallel Scientific Applications" by X. Tian et al. (2013)**
  - [Link](https://ieeexplore.ieee.org/document/6569811)
- **"Structure layout optimizations by Michael Lai**
  - [Link](https://patents.google.com/patent/US8910135B2/en)

## Additional Online Resources

- **Intel C++ Compiler Documentation on Pragma Directives**
  - [Link](https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-8/pragmas.html)
- **Oracle C User's Guide on Pragma Directives**
  - [Link](https://docs.oracle.com/cd/E19205-01/819-5265/bjabt/index.html)
- **CPPReference on Data Structure Alignment**
  - [Link](https://en.cppreference.com/w/c/language/object#Alignment)
- **IBM Knowledge Center on #pragma pack**
  - [Link](https://www.ibm.com/docs/en/zos/2.5.0?topic=reference-zos-xl-cc-pragmas)
 
## Books

- **"C: A Reference Manual" by Samuel P. Harbison and Guy L. Steele Jr.**
  - ISBN: 013089592X
- **"Expert C Programming: Deep C Secrets" by Peter van der Linden**
  - ISBN: 0131774298
- **"Computer Systems: A Programmer's Perspective" by Randal E. Bryant and David R. O'Hallaron**
  - ISBN: 013409266X
- **"The C Programming Language" by Brian W. Kernighan and Dennis M. Ritchie**
  - ISBN: 0131103628
- **"C in a Nutshell" by Peter Prinz and Tony Crawford**
  - ISBN: 1491904755

## For Deep Dive into Implementation

- **"Compiler Design in C" by Allen I. Holub**
  - ISBN: 0131550454
- **"Engineering a Compiler" by Keith Cooper and Linda Torczon**
  - ISBN: 012088478X

## Standards

- **ISO/IEC 9899:2018 (C17 standard)**
  - Section 6.10.6 covers pragma directives in the C language standard
  - Not freely available, but it's the authoritative source on C language features

```c
#pragma pack(1)
struct Example {
    char a;
    int b;
};
```

## Note

Pragma directives are often compiler-specific extensions to the C standard, so their behavior can vary between different compilers. Always refer to the documentation for your specific compiler when using pragma directives in your code.
