#include "stub.h"

/* int gosave(Gobuf *); */
function(gosave):
  /* Save PC */
  mov (%rsp), %rdx
  mov %rdx, 8(%rdi)

  /* 
   * Instead of doing many `push`s, each of which does 
   * one SP increment and one data movement, we allocate
   * the stack once for all movements.
   */
  sub %rsp, $48

  /* Save callee-saved registers */
  mov %rbx,  0(%rsp)
  mov %rbp,  8(%rsp)
  mov %r12, 16(%rsp)
  mov %r13, 24(%rsp)
  mov %r14, 32(%rsp)
  mov %r15, 40(%rsp)

  /* Save SP */
  mov %rsp, (%rdi)

  /* Always return 0 */
  xor %rax, %rax
  ret


/* void gogo(Gobuf *); */
function(gogo):
  /* Restore SP */
  mov (%rdi), %rsp

  /* Restore callee-saved registers */
  mov 40(%rsp), %r15
  mov 32(%rsp), %r14
  mov 24(%rsp), %r13
  mov 16(%rsp), %r12
  mov  8(%rsp), %rbp
  mov  0(%rsp), %rbx

  /* Restore the stack to where `gosave` is called */
  add %rsp, $48

  /* Return 1 for `gosave` */
  mov $1, %rax

  /* Restore PC */
  mov 8(%rdi), %rdx
  jmp *%rdx
