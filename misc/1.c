extern void foo() __asm__("foo");

int bar() {
  foo();
}
