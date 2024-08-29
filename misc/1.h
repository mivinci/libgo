struct A {
  int a;
  int b;
};

#define N __builtin_offsetof(struct A, b)
