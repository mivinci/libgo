	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 13, 0	sdk_version 14, 2
	.globl	_setg                           ## -- Begin function setg
	.p2align	4, 0x90
_setg:                                  ## @setg
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, %rcx
	movq	_g@TLVP(%rip), %rdi
	callq	*(%rdi)
	movq	%rcx, (%rax)
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_getg                           ## -- Begin function getg
	.p2align	4, 0x90
_getg:                                  ## @getg
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	_g@TLVP(%rip), %rdi
	callq	*(%rdi)
	movq	(%rax), %rax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
.tbss _g$tlv$init, 8, 3                 ## @g

	.section	__DATA,__thread_vars,thread_local_variables
_g:
	.quad	__tlv_bootstrap
	.quad	0
	.quad	_g$tlv$init

.subsections_via_symbols
