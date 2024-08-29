#ifndef LIBGO_CORE_LOCK_H_
#define LIBGO_CORE_LOCK_H_

typedef struct Lock Lock;

struct Lock {
  int key;
};

#endif // LIBGO_CORE_LOCK_H_
