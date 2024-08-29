#include "asm.h"

.text
.globl goboot
goboot:
  // int goboot(Go_Func entry, int argc, char **argv)
  // Extend stack
  push %rbp
  mov  %rsp, %rbp
  sub  $32,  %rsp

  // Save entry and argc to stack, we don't save
  // argv because rdx is not used during the call
  mov  %rdi, 0(%rsp)
  mov  %rsi, 8(%rsp)

  // Setup globals
  mov  %rsp, %rdi
  call setup_g0m0
  call setup_sys
  call setup_sched

  // Create the first G to run `entry`
  mov  8(%rsp), %rsi  # rdx = argc
  mov  0(%rsp), %rdi  # rsi = entry
  call gospawn
  
  // Start this M (m0 - the first scheduling thread)
  // and we are never coming back unless error.
  call mstart

  // This is unreachable if _mstart works right. And
  // if it doesn't, we have to notify the caller to
  // exit by returning 1 to it. (we don't use -1 or 
  // 0 here because the caller may return this value
  // directly in the main function)

  // Release stack
  add  $32,  %rsp
  pop  %rbp
  // Return 1
  mov  $1,   %rax
  ret


.text
.globl gospawn 
gospawn:
  // void gospawn(Go_Func fn, ...)
  // We are now in a G stack and are going to save all 
  // possible arguments taken by registers to the stack
  // and the return address (RA) on top of it as if all 
  // arguments are passed to us by stack. Therefore, the 
  // G stack MUST always have at least 6*8 bytes left when
  // calling us. Go does this at compile time, but we have
  // no compilers do this for us.

  // Extend stack
  // Save RA
  sub  $48,       %rsp
  mov  48(%rsp),  %rax 
  mov  %rax,     (%rsp)

  // Save arguments taken by registers to make it look
  // like all arguements are passed by stack
  mov  %rdi,   8(%rsp)
  mov  %rsi,  16(%rsp)
  mov  %rdx,  24(%rsp)
  mov  %rcx,  32(%rsp)
  mov  %r8,   40(%rsp)
  mov  %r9,   48(%rsp)

  // argp = (%rsp - 8)  # skip RA
  lea 8(%rsp), %rdx

  // size = %rbp - argp
  mov  %rbp,  %rax
  sub  %rdx,  %rax

  // if (size > 256) {
  //   size = 256;
  // }
  cmp  $256,  %rax
  jl   1f
  mov  $256,  %rax

  // if (size < 48) {
  //   size = 48;
  // }
  cmp  $48,  %rax
  jg   1f
  mov  $48,  %rax
  
1:
  // callerpc = (RA - 5)
  // 5 is the size that instruction `call gospawn1` takes
  // on x86_64
  mov  (%rsp), %rcx
  sub  $5,     %rcx
  
  // gospawn1(fn, size, argp, callerpc)
  mov  %rax,  %rsi  # size
  call gospawn1

  // Restore RA
  // Release stack 
  mov  (%rsp), %rax
  mov  %rax,   48(%rsp)
  add  $48,    %rsp
  ret


.text
.globl gosave
gosave:
  mov $0, %rax
  ret


.text
.globl gogo
gogo:
  mov  $0, %rax
  ret


.text
.globl gostart
gostart:
  // void gostart(Gobuf *buf, void *fn)
  // Here we're going to make it look like goexit called
  // fn, so fn will go back to goexit when it returns.
  // 
  // gospawn_m has already given us:
  // gobuf.sp = argp
  // gobuf.pc = goexit

  // buf.sp -= 1
  // buf.sp = buf.pc (goexit)
  lea  (-8+Gobuf_sp)(%rdi),  %rdx              # %rdx = buf.sp - 1
  mov  %rdx,                 (Gobuf_sp)(%rdi)  # buf.sp = %rbx
  mov  Gobuf_pc(%rdi),       %rdx              # %rbx = buf.pc
  mov  %rdx,                 Gobuf_sp(%rdi)    # buf.sp = %rbx 

  // buf.pc = fn
  mov  %rsi,     Gobuf_pc(%rdi)  # buf.pc = fn

  // buf.ctx = buf.sp + 1
  lea  (8+Gobuf_sp)(%rdi),  %rax
  mov  %rax,                Gobuf_ctx(%rdi)

  // buf.g = NULL
  mov  $0,    %rax
  mov  %rax,  (Gobuf_g)(%rdi)
  
  ret


.text
.globl mcall
mcall:
  // void mcall(void(*fn)(G *))
  mov  %rdi, %rax  # %rax = fn

  mov  %fs:g@tpoff,  %rdi  # %rdi = g
  mov  G_m(%rdi),    %rbx  # %rbx = g.m
  mov  M_g0(%rbx),   %rcx  # %rcx = g.m.g0
  cmp  %rdi,         %rcx
  je   1f

  // Save SP and the current G
  mov  %rsp,  (G_sched+Gobuf_sp)(%rdi)  # g.sched.sp = %rsp
  mov  %rdi,  (G_sched+Gobuf_g)(%rcx)   # g.m.g0.sched.g = g

  // Switch to G0
  mov  (G_sched+Gobuf_sp)(%rcx),  %rsp         # %rsp = g.m.g0.sched.sp
  mov  %rcx,                      %fs:g@tpoff  # g = g.m.g0

  call *%rax

  // Restore SP and G we just switched from. We have
  // to re-get the current G in case fn changed %rcx
  mov  %fs:g@tpoff,               %rdi         # %rdi = g (g0)
  mov  (G_sched+Gobuf_g)(%rdi),   %rbx         # %rbx = g0.sched.g
  mov  (G_sched+Gobuf_sp)(%rbx),  %rsp         # %rsp = g0.sched.g.sched.sp
  mov  %rbx,                      %fs:g@tpoff  # g = g0.sched.g
  ret

1:
  call *%rax
  ret
