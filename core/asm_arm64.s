// int goboot(Go_Func, int, char **)
.text
.globl goboot
goboot:
  // Allocate stack frame
  sub sp, sp, #32
  stp x29, x30, [sp, #16]

  // Save entry and argc to stack, we don't save
  // argv because x2 is not used during the call
  stp x0, x1, [sp, #32]

  // Setup globals
  mov x0, sp
  bl  _setup_g0m0
  bl  _setup_sys
  bl  _setup_sched
  
  // Restore entry and argc from stack
  ldp x0, x1, [sp, #32]

  // Create the first G to run `entry`
  bl  _gospawn

  // Start this M (m0 - the first scheduling thread)
  bl  _mstart

  // This is unreachable if _mstart works right. And
  // if it doesn't, we have to notify the caller to
  // exit by returning 1 to it. (we don't use -1 or 
  // 0 here because the caller may return this value
  // directly in the main function)

  // Release stack frame
  ldp x29, x30, [sp, #16]
  add sp, sp, #32

  // Return 1
  mov x0, #1
  ret


.text
.globl gosave
gosave:
  mov x0, #0
  ret


.text
.globl gogo
gogo:
  mov x0, #0
  ret


.text
.globl gostart
gostart:
  // Gobuf.ctx = Gobuf.sp 
  ldr x2,  [x0]      # rdx <- Gobuf.sp
  str x2,  [x0, 24]  # Gobuf.ctx <- rdx

  // Gobuf.lr = goexit
  ldr x2,  [x0, 8]
  str x2,  [x0, 16]

  // Gobuf.pc = fn
  str x1,  [x0, 8]   # Gobuf.pc <- fn
  
  ret
