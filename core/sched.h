#ifndef LIBGO_CORE_SCHED_H_
#define LIBGO_CORE_SCHED_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct G G;
typedef struct M M;
typedef struct P P;

struct G {
  M *m;
};

struct M {
  P *p;
};

struct P {
  M *m;
};

#ifdef __cplusplus
};
#endif

#endif // LIBGO_CORE_SCHED_H_
