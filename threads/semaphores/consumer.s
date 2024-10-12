	.file	"consumer.c"
	.text
	.section	.rodata
.LC0:
	.string	"/my_shared_memory"
.LC1:
	.string	"shm_open"
.LC2:
	.string	"ftruncate"
.LC3:
	.string	"mmap"
.LC4:
	.string	"/sem_producer"
.LC5:
	.string	"/sem_consumer"
.LC6:
	.string	"sem_open"
.LC7:
	.string	"Consumed: %s\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	$438, %edx
	movl	$66, %esi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	call	shm_open@PLT
	movl	%eax, -28(%rbp)
	cmpl	$-1, -28(%rbp)
	jne	.L2
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %edi
	call	exit@PLT
.L2:
	movl	-28(%rbp), %eax
	movl	$256, %esi
	movl	%eax, %edi
	call	ftruncate@PLT
	cmpl	$-1, %eax
	jne	.L3
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %edi
	call	exit@PLT
.L3:
	movl	-28(%rbp), %eax
	movl	$0, %r9d
	movl	%eax, %r8d
	movl	$1, %ecx
	movl	$3, %edx
	movl	$256, %esi
	movl	$0, %edi
	call	mmap@PLT
	movq	%rax, -24(%rbp)
	cmpq	$-1, -24(%rbp)
	jne	.L4
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %edi
	call	exit@PLT
.L4:
	movl	$0, %ecx
	movl	$438, %edx
	movl	$64, %esi
	leaq	.LC4(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	sem_open@PLT
	movq	%rax, -16(%rbp)
	movl	$1, %ecx
	movl	$438, %edx
	movl	$64, %esi
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	sem_open@PLT
	movq	%rax, -8(%rbp)
	cmpq	$0, -16(%rbp)
	je	.L5
	cmpq	$0, -8(%rbp)
	jne	.L6
.L5:
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %edi
	call	exit@PLT
.L6:
	movl	$0, -32(%rbp)
	jmp	.L7
.L8:
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	sem_wait@PLT
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC7(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-24(%rbp), %rax
	movl	$256, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	sem_post@PLT
	movl	$1, %edi
	call	sleep@PLT
	addl	$1, -32(%rbp)
.L7:
	cmpl	$9, -32(%rbp)
	jle	.L8
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	sem_close@PLT
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	sem_close@PLT
	leaq	.LC4(%rip), %rax
	movq	%rax, %rdi
	call	sem_unlink@PLT
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	sem_unlink@PLT
	movq	-24(%rbp), %rax
	movl	$256, %esi
	movq	%rax, %rdi
	call	munmap@PLT
	movl	-28(%rbp), %eax
	movl	%eax, %edi
	call	close@PLT
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	call	shm_unlink@PLT
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
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
