	.file	"blocksize.c"
	.text
	.section	.rodata
.LC0:
	.string	"fstat stdout failed"
.LC1:
	.string	"test.txt"
.LC2:
	.string	"open failed"
.LC3:
	.string	"fstat file failed"
.LC4:
	.string	"stdout block size: %ld\n"
.LC5:
	.string	"file block size: %ld\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$320, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	-304(%rbp), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	fstat@PLT
	cmpl	$-1, %eax
	jne	.L2
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %eax
	jmp	.L6
.L2:
	movl	$420, %edx
	movl	$65, %esi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	open@PLT
	movl	%eax, -308(%rbp)
	cmpl	$-1, -308(%rbp)
	jne	.L4
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	$1, %eax
	jmp	.L6
.L4:
	leaq	-160(%rbp), %rdx
	movl	-308(%rbp), %eax
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	fstat@PLT
	cmpl	$-1, %eax
	jne	.L5
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	call	perror@PLT
	movl	-308(%rbp), %eax
	movl	%eax, %edi
	call	close@PLT
	movl	$1, %eax
	jmp	.L6
.L5:
	movq	-248(%rbp), %rdx
	movq	stderr(%rip), %rax
	leaq	.LC4(%rip), %rcx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movq	-104(%rbp), %rdx
	movq	stderr(%rip), %rax
	leaq	.LC5(%rip), %rcx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf@PLT
	movl	-308(%rbp), %eax
	movl	%eax, %edi
	call	close@PLT
	movl	$0, %eax
.L6:
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L7
	call	__stack_chk_fail@PLT
.L7:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
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
