	.file	"aslr.c"
	.text
	.globl	g_memory_layout
	.bss
	.align 32
	.type	g_memory_layout, @object
	.size	g_memory_layout, 16000040
g_memory_layout:
	.zero	16000040
	.text
	.globl	init_memory_layout
	.type	init_memory_layout, @function
init_memory_layout:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$8, %rsp
	.cfi_offset 3, -24
	movl	$16000040, %edx
	movl	$0, %esi
	leaq	g_memory_layout(%rip), %rax
	movq	%rax, %rdi
	call	memset@PLT
	movl	$0, %edi
	call	time@PLT
	movl	%eax, %ebx
	call	getpid@PLT
	sall	$16, %eax
	xorl	%ebx, %eax
	movl	%eax, %edi
	call	srand@PLT
	nop
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	init_memory_layout, .-init_memory_layout
	.globl	randomize_address
	.type	randomize_address, @function
randomize_address:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	call	rand@PLT
	cltq
	movl	$0, %edx
	divq	-16(%rbp)
	movq	-8(%rbp), %rax
	addq	%rdx, %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	randomize_address, .-randomize_address
	.globl	allocate_random_page
	.type	allocate_random_page, @function
allocate_random_page:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
.L5:
	call	rand@PLT
	movslq	%eax, %rdx
	imulq	$1125899907, %rdx, %rdx
	shrq	$32, %rdx
	sarl	$18, %edx
	movl	%eax, %ecx
	sarl	$31, %ecx
	subl	%ecx, %edx
	movl	%edx, -4(%rbp)
	movl	-4(%rbp), %edx
	imull	$1000000, %edx, %edx
	subl	%edx, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	cltq
	salq	$4, %rax
	movq	%rax, %rdx
	leaq	8+g_memory_layout(%rip), %rax
	movl	(%rdx,%rax), %eax
	testl	%eax, %eax
	jne	.L5
	movl	-4(%rbp), %eax
	cltq
	salq	$4, %rax
	movq	%rax, %rdx
	leaq	8+g_memory_layout(%rip), %rax
	movl	$1, (%rdx,%rax)
	movl	-4(%rbp), %eax
	sall	$12, %eax
	cltq
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	movq	%rdx, %rcx
	salq	$4, %rcx
	leaq	g_memory_layout(%rip), %rdx
	movq	%rax, (%rcx,%rdx)
	movl	-4(%rbp), %eax
	cltq
	salq	$4, %rax
	movq	%rax, %rdx
	leaq	g_memory_layout(%rip), %rax
	movq	(%rdx,%rax), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	allocate_random_page, .-allocate_random_page
	.globl	apply_aslr
	.type	apply_aslr, @function
apply_aslr:
.LFB9:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$65536, %esi
	movl	$4194304, %edi
	call	randomize_address
	movq	%rax, 16000000+g_memory_layout(%rip)
	movl	$65536, %esi
	movl	$6291456, %edi
	call	randomize_address
	movq	%rax, 16000008+g_memory_layout(%rip)
	movl	$1048576, %esi
	movl	$8388608, %edi
	call	randomize_address
	movq	%rax, 16000016+g_memory_layout(%rip)
	movl	$268435456, %esi
	movl	$4143972352, %eax
	movq	%rax, %rdi
	call	randomize_address
	movq	%rax, 16000032+g_memory_layout(%rip)
	movl	$131072, %esi
	movabsq	$140737488216064, %rax
	movq	%rax, %rdi
	call	randomize_address
	movq	%rax, 16000024+g_memory_layout(%rip)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	apply_aslr, .-apply_aslr
	.globl	sim_malloc
	.type	sim_malloc, @function
sim_malloc:
.LFB10:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	$0, %eax
	call	allocate_random_page
	movq	%rax, -8(%rbp)
	call	rand@PLT
	cltq
	movl	$4096, %edx
	movq	%rdx, %rcx
	subq	-24(%rbp), %rcx
	movl	$0, %edx
	divq	%rcx
	movq	-8(%rbp), %rax
	addq	%rdx, %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	sim_malloc, .-sim_malloc
	.globl	sim_alloca
	.type	sim_alloca, @function
sim_alloca:
.LFB11:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$24, %rsp
	.cfi_offset 3, -24
	movq	%rdi, -24(%rbp)
	movq	16000024+g_memory_layout(%rip), %rbx
	call	rand@PLT
	cltq
	movl	$131072, %edx
	movq	%rdx, %rcx
	subq	-24(%rbp), %rcx
	movl	$0, %edx
	divq	%rcx
	movq	%rbx, %rax
	subq	%rdx, %rax
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	sim_alloca, .-sim_alloca
	.globl	load_shared_library
	.type	load_shared_library, @function
load_shared_library:
.LFB12:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	16000032+g_memory_layout(%rip), %rax
	movl	$268435456, %esi
	movq	%rax, %rdi
	call	randomize_address
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	load_shared_library, .-load_shared_library
	.section	.rodata
.LC0:
	.string	"Stack variable address: %p\n"
	.text
	.globl	stack_function
	.type	stack_function, @function
stack_function:
.LFB13:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	-12(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	movq	-8(%rbp), %rax
	subq	%fs:40, %rax
	je	.L15
	call	__stack_chk_fail@PLT
.L15:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	stack_function, .-stack_function
	.section	.rodata
.LC1:
	.string	"Process ID: %d\n"
.LC2:
	.string	"Text segment start: %p\n"
.LC3:
	.string	"Data segment start: %p\n"
.LC4:
	.string	"Heap start: %p\n"
.LC5:
	.string	"Stack start: %p\n"
.LC6:
	.string	"mmap region start: %p\n"
.LC7:
	.string	"\nStack Randomization:"
.LC8:
	.string	"\nHeap Randomization:"
.LC9:
	.string	"Heap allocation %d: %p\n"
	.align 8
.LC10:
	.string	"\nShared Library Randomization:"
.LC11:
	.string	"example_lib.so"
	.align 8
.LC12:
	.string	"Shared library %d loaded at: %p\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB14:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	$0, %eax
	call	init_memory_layout
	movl	$0, %eax
	call	apply_aslr
	call	getpid@PLT
	movl	%eax, %esi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	16000000+g_memory_layout(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	16000008+g_memory_layout(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	16000016+g_memory_layout(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC4(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	16000024+g_memory_layout(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	16000032+g_memory_layout(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	leaq	.LC7(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movl	$0, -28(%rbp)
	jmp	.L17
.L18:
	movl	$0, %eax
	call	stack_function
	addl	$1, -28(%rbp)
.L17:
	cmpl	$2, -28(%rbp)
	jle	.L18
	leaq	.LC8(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movl	$0, -24(%rbp)
	jmp	.L19
.L20:
	movl	$4, %edi
	call	sim_malloc
	movq	%rax, -8(%rbp)
	movl	-24(%rbp), %eax
	leal	1(%rax), %ecx
	movq	-8(%rbp), %rax
	movq	%rax, %rdx
	movl	%ecx, %esi
	leaq	.LC9(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -24(%rbp)
.L19:
	cmpl	$2, -24(%rbp)
	jle	.L20
	leaq	.LC10(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movl	$0, -20(%rbp)
	jmp	.L21
.L22:
	leaq	.LC11(%rip), %rax
	movq	%rax, %rdi
	call	load_shared_library
	movq	%rax, -16(%rbp)
	movl	-20(%rbp), %eax
	leal	1(%rax), %ecx
	movq	-16(%rbp), %rax
	movq	%rax, %rdx
	movl	%ecx, %esi
	leaq	.LC12(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	addl	$1, -20(%rbp)
.L21:
	cmpl	$2, -20(%rbp)
	jle	.L22
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE14:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
