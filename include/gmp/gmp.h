#ifndef GMP_H
#define GMP_H

#ifdef __WIN32__
#define GMP_EXPORT __declspec(dllexport)
#else
#define GMP_EXPORT
#endif

#define GMP_go(f, a) GMP_spawn((void *)f, (void *)a, 0)

typedef void (*GMP_Func)(void *);

GMP_EXPORT void GMP_init(int);
GMP_EXPORT void GMP_spawn(GMP_Func, void *, int);
GMP_EXPORT void GMP_yield(void);
GMP_EXPORT void GMP_exit(int);

#endif
