#ifndef GMP_H
#define GMP_H

typedef void (*GMP_Func)(void *);

void GMP_spawn(GMP_Func, void *, int, int);

#endif
