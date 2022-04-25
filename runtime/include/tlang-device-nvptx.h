#ifndef TLANG_RUNTIME_DEVICE_NVPTX_H
#define TLANG_RUNTIME_DEVICE_NVPTX_H

#include <cstdint>

struct Vec3 {
  int x;
  int y;
  int z;
};
using address_type = int8_t*;
extern "C" {
address_type __tlang_device_map(int kind, address_type address, uint64_t size);
void __tlang_device_sync(int id);
void __tlang_device_run_kernel(const void *fn, Vec3 tensor_dim, Vec3 matrix_dim, void **args);
}

template <typename ...Args>
void __tlang_device_run_kernel_rt(const void *fn, Vec3 tensor_dim, Vec3 matrix_dim, Args ...args) {
  void *kernel_args[] = { static_cast<void*>(&args)... };
  __tlang_device_run_kernel(fn, tensor_dim, matrix_dim, kernel_args);
}

#endif
