#include <stdatomic.h>

atomic_int acnt;
void foo(int x) {
  acnt += x;
}
