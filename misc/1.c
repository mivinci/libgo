struct G {};

static __thread struct G *g;

void setg(struct G *gp) {
  g = gp;
}

struct G *getg(void) {
  return g;
}
