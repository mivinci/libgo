/* int gosave(Gobuf *); setjmp */
.text
.globl gosave
gosave:
  mov x1,  sp
  str x1, [x0, #0]
  str lr, [x0, #8]
  ldr x1, [x0, #16]

  stp x19, x20, [x1, #0]
  stp x21, x22, [x1, #16]
  stp x23, x24, [x1, #32]
  stp x25, x26, [x1, #48]
  stp x27, x28, [x1, #64]
  stp x29, x30, [x1, #80]

  mov x0, #0
  ret


/* void gogo(Gobuf *); longjmp */
.text
.globl gogo
gogo:
  ldr x1, [x0, #0]
  mov sp,  x1
  ldr x2, [x0, #8]
  ldr x1, [x0, #16]
  
  ldp x19, x20, [x1, #0]
  ldp x21, x22, [x1, #16]
  ldp x23, x24, [x1, #32]
  ldp x25, x26, [x1, #48]
  ldp x27, x28, [x1, #64]
  ldp x29, x30, [x1, #80]

  mov x0, #1
  br  x2

