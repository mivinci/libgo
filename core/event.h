#ifndef GMP_EVENT_H
#define GMP_EVENT_H

typedef struct Event Event;

struct Event {
  int fd;
  long when;
  long period;
  void (*f)(void *, long);
  void *arg;
  struct G *g;
};

#endif
