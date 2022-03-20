#ifndef __TLANG_PARALLEL_RUNTIME_H__
#define __TLANG_PARALLEL_RUNTIME_H__

#ifdef __cplusplus
extern "C" {
#endif
void __tlang_create_parallel(void* (*exec)(void*), void *data, int hasBarrier);
void __tlang_init_parallel(int tid);
void __tlang_exit_parallel();
void __tlang_sync();
int __tlang_tid();
int __tlang_nt();
#ifdef __cplusplus
}
#endif
#endif
