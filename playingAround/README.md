# Building the Hello World Executable

To understand how to convert `hello.c` into an executable `hello.out` using GCC, follow these steps:

## Step 1: Preprocessing

```bash
gcc -E hello.c -o hello.i
```

This step preprocesses `hello.c` and outputs the expanded source code into `hello.i`.

## Step 2: Compilation

```bash
gcc -S hello.i -o hello.s
```

Compile the preprocessed file to create an assembly code file `hello.s`.

## Step 3: Assembly

```bash
gcc -c hello.s -o hello.o
```

Assemble the assembly code into machine code, producing an object file `hello.o`.


## Step 4: Linking


```bash
gcc hello.o -o hello.out
```

Link the object file to create the final executable `hello.out`.
