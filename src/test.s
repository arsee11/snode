	.file	"test.cpp"
	.globl	addr
	.bss
	.align 8
	.type	addr, @object
	.size	addr, 8
addr:
	.zero	8
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movabsq	$1224979244673662976, %rax
	movq	%rax, -24(%rbp)
	leaq	-24(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, addr(%rip)
	movl	$addr, %eax
	movq	(%rax), %rax
	movq	%rax, -16(%rbp)
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	xorq	%fs:40, %rdx
	je	.L3
	call	__stack_chk_fail
.L3:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
