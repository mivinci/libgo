#ifndef GMP_POLLER_H
#define GMP_POLLER_H

#include "sched.h"

void Poller_init(void);
G *Poller_poll(uint64_t);
int Poller_open(Event *);

#endif
