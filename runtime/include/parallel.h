#ifndef TLANG_PARALLEL_RUNTIME_H
#define TLANG_PARALLEL_RUNTIME_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
  int x;
  int y;
} __Tlang_Range;
void __tlang_create_parallel(void* (*exec)(void*), void *data, int hasBarrier);
void __tlang_init_parallel(int tid);
void __tlang_exit_parallel();
void __tlang_sync();
int __tlang_tid();
int __tlang_nt();
__Tlang_Range __tlang_loop_partition(int begin, int end);
#ifdef __cplusplus
}
#endif
#endif
