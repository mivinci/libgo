#ifndef GMP_ASM_H
#define GMP_ASM_H
#ifdef __ASSEMBLY__

#define function(name) \
.text \
.globl name \
.type name, @function \
name


#endif /* __ASSEMBLY__ */
#endif /* GMP_ASM_H */
