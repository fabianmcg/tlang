#include "../runtime/include/parallel.h"
#include <iostream>

thread_local int x[10];

struct X {
  int *data;
  int tid;
};

int* getX() {
  return x;
}

void* function(void *threadData) {
  X *ptr = (X*) threadData;
  __tlang_init_parallel(ptr->tid);
  std::cerr << ptr->tid << " " << __tlang_tid() << " " << x << " " << ptr->data << " " << getX() << std::endl;
  __tlang_exit_parallel();
  return nullptr;
}

int main() {
  __tlang_create_parallel(function, x, 0);
  return 0;
}
