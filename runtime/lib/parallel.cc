#include "parallel.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <pthread.h>
#include <thread>
#include <vector>

namespace {
thread_local int tid = 0;
struct ParallelRegion {
  ParallelRegion() = default;
  inline void init(int numThreads, bool hasBarrier) {
    if (numThreads < 1)
      numThreads = 0;
    this->numThreads = numThreads;
    if (numThreads > 1) {
      this->hasBarrier = hasBarrier;
      if (hasBarrier)
        pthread_barrier_init(&barrier, nullptr, numThreads);
    }
  }
  inline void destroy() {
    if (hasBarrier)
      pthread_barrier_destroy(&barrier);
    numThreads = 0;
    hasBarrier = false;
  }
  inline void sync() {
    if (hasBarrier)
      pthread_barrier_wait(&barrier);
  }
  inline int getNumThreads() const {
    return numThreads;
  }
protected:
  int numThreads = 0;
  bool hasBarrier = false;
  pthread_barrier_t barrier { };
};
ParallelRegion region { };
struct threadData {
  void *data { };
  int tid { };
};
int getNumThreads() {
  if (char *env = getenv("TLANG_NUM_THREADS")) {
    int nt = atoi(env);
    if (nt > 0)
      return nt;
  }
  return std::thread::hardware_concurrency();
}
}
void __tlang_create_parallel(void* (*exec)(void*), void *data, int hasBarrier) {
  region.init(getNumThreads(), hasBarrier);
  int nt = region.getNumThreads();
  if (nt) {
    std::vector<std::pair<pthread_t, threadData>> threadArguments(nt);
    for (int i = 0; i < nt; ++i) {
      threadArguments[i].second = threadData { data, i };
      if (int rc = pthread_create(&(threadArguments[i].first), NULL, exec, (void*) &(threadArguments[i].second)))
        std::cerr << "Failed to create thread[" << i << "] with error code: " << rc << std::endl;
    }
    for (int i = 0; i < nt; ++i)
      pthread_join(threadArguments[i].first, NULL);
  }
  region.destroy();
}
void __tlang_init_parallel(int id) {
  tid = id;
//  std::string tmp = "I:" + std::to_string(tid) + " " + std::to_string(region.getNumThreads());
//  std::cerr << tmp << std::endl;
}
void __tlang_exit_parallel() {
//  std::string tmp = "E:" + std::to_string(tid) + " " + std::to_string(region.getNumThreads());
//  std::cerr << tmp << std::endl;
  pthread_exit(NULL);
}
void __tlang_sync() {
  region.sync();
}
int __tlang_tid() {
  return tid;
}
int __tlang_nt() {
  return region.getNumThreads();
}
__Tlang_Range    __tlang_loop_partition(int begin, int end)    {
  int nt = region.getNumThreads();
  int size = end - begin, start, stop;
  size = (size + nt - 1) / nt;
  start = tid * size + begin;
  stop = start + size;
  if (end <= start)
    start = stop = 0;
  if (end <= stop)
    stop = end;
  return __Tlang_Range { start, stop };
}
