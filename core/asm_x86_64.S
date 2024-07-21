#include "asm.h"

function(setg):
  mov %rdi, %fs:g

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


/* void gogo(Gobuf *, void *); */
function(gogo):
  /* Restore SP */
  mov 0(%rdi), %rdx
  mov %rdx,    %rsp

  /* Take PC */
  mov 8(%rdi), %rdx

  /* Restore callee-saved register */
  mov 16(%rdi), %rbx
  mov 24(%rdi), %rbp
  mov 32(%rdi), %r12
  mov 40(%rdi), %r13
  mov 48(%rdi), %r14
  mov 56(%rdi), %r15

  /* 
   * If the second argument passed to `gogo` is not zero,
   * meaning the scheduler is resuming a function for the
   * first time, then we have to pass that second argument
   * to the function.
   */
  test %rsi, %rsi
  jz   1f 
  mov %rsi, %rdi
1:
  mov $1, %rax
  jmp *%rdx
