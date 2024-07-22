#ifndef GMP_STUB_H
#define GMP_STUB_H
#ifdef __ASSEMBLY__

#define function(name)                                                         \
  .text;                                                                       \
  .globl name;                                                                 \
  .type name, @function;                                                       \
  name

#endif /* __ASSEMBLY__ */

#define PTR_SIZE (sizeof(void *))
#define STK_ALIGN 16
#define STK_MIN 2048

#define align_up(n, a) ((n + a - 1) & (~(a - 1)))
#define align_down(n, a) ((n) & (~(a - 1)))

#define len(p) (sizeof(p) / sizeof(p[0]))

#endif /* GMP_STUB_H */
