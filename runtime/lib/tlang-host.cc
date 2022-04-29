#include <tlang-host.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <pthread.h>

namespace {
int getDim() {
  if (char *env = getenv("TLANG_TENSOR_DIM")) {
    int nt = atoi(env);
    if (nt > 0)
      return nt;
  }
  return std::thread::hardware_concurrency();
}
thread_local int tid = 0;
struct TlangHostRuntime {
  TlangHostRuntime() {
    default_num_tensors = getDim();
    init(default_num_tensors);
  }
  ~TlangHostRuntime() {
    destroy();
  }
  inline void init(int num_tensors) {
    if (num_tensors < 1)
      num_tensors = default_num_tensors;
    if (num_tensors > 1)
      if (this->num_tensors != num_tensors) {
        this->num_tensors = num_tensors;
        if (has_barrier)
          pthread_barrier_destroy(&barrier);
        has_barrier = true;
        pthread_barrier_init(&barrier, nullptr, this->num_tensors);
      }
  }
  inline void destroy() {
    if (has_barrier)
      pthread_barrier_destroy(&barrier);
    num_tensors = 0;
    has_barrier = false;
  }
  inline void sync() {
    if (has_barrier)
      pthread_barrier_wait(&barrier);
  }
  inline int dim() const {
    return num_tensors;
  }
protected:
  int default_num_tensors = 0;
  int num_tensors = 0;
  bool has_barrier = false;
  pthread_barrier_t barrier { };
};
std::unique_ptr<TlangHostRuntime> region { };
inline TlangHostRuntime& get_rt_region() {
  if (!region)
    region = std::unique_ptr<TlangHostRuntime>(new TlangHostRuntime());
  return *region;
}
}
void __tlang_host_create_parallel(int num_tensors, void* (*kernel)(void*), void *arguments) {
  get_rt_region().init(num_tensors);
  int nt = get_rt_region().dim();
  if (nt) {
    std::vector<std::pair<pthread_t, __tlang_host_arguments>> thread_arguments(nt);
    for (int i = 0; i < nt; ++i) {
      thread_arguments[i].second = __tlang_host_arguments { i, arguments };
      if (int rc = pthread_create(&(thread_arguments[i].first), NULL, kernel, (void*) &(thread_arguments[i].second)))
        std::cerr << "Failed to create thread[" << i << "] with error code: " << rc << std::endl;
    }
    for (int i = 0; i < nt; ++i)
      pthread_join(thread_arguments[i].first, NULL);
  }
}
void __tlang_host_init(int id) {
  tid = id;
}
void __tlang_host_exit() {
  pthread_exit(NULL);
}
void __tlang_host_sync() {
  get_rt_region().sync();
}
int __tlang_host_tid() {
  return tid;
}
int __tlang_host_dim() {
  return get_rt_region().dim();
}
