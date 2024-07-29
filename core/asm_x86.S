/* int gosave(Gobuf *); */
.text
.globl gosave
gosave:
  mov %rsp, 0(%rdi)
  mov (%rsp), %rdx
  mov %rdx, 8(%rdi)

  mov 16(%rdi), %rdx

  mov %rbx,  0(%rdx)
  mov %rbp,  8(%rdx)
  mov %r12, 16(%rdx)
  mov %r13, 24(%rdx)
  mov %r14, 32(%rdx)
  mov %r15, 40(%rdx)

  xor %rax, %rax
  ret


/* void gogo(Gobuf *); */
.text
.globl gogo
gogo:
  mov (%rdi), %rsp
  mov 16(%rdi), %rdx

  mov 40(%rdx), %r15
  mov 32(%rdx), %r14
  mov 24(%rdx), %r13
  mov 16(%rdx), %r12
  mov  8(%rdx), %rbp
  mov  0(%rdx), %rbx

  mov $1, %rax

  mov 8(%rdi), %rdx
  jmp *%rdx
