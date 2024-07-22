#include "stub.h"


/* int gosave(Gobuf *); */
function(gosave):
  /* Save SP, the one before the return address is pushed by `call` */
  lea 8(%rsp), %rdx
  mov %rdx,    0(%rdi)

  /* Save PC */
  mov (%rsp), %rdx
  mov %rdx,   8(%rdi)

  /* Save callee-saved registers */
  mov %rbx, 16(%rdi)
  mov %rbp, 24(%rdi)
  mov %r12, 32(%rdi) 
  mov %r13, 40(%rdi)
  mov %r14, 48(%rdi)
  mov %r15, 56(%rdi)

  /* Always return zero */
  xor %rax, %rax
  ret


/* void gogo(Gobuf *); */
function(gogo):
  /* Restore SP */
  mov 0(%rdi), %rdx
  mov %rdx,    %rsp

  /* Load PC */
  mov 8(%rdi), %rdx

  /* Restore callee-saved register */
  mov 16(%rdi), %rbx
  mov 24(%rdi), %rbp
  mov 32(%rdi), %r12
  mov 40(%rdi), %r13
  mov 48(%rdi), %r14
  mov 56(%rdi), %r15

  mov $1, %rax
  jmp *%rdx
