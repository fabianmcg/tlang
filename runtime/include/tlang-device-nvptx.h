#ifndef TLANG_RUNTIME_DEVICE_NVPTX_H
#define TLANG_RUNTIME_DEVICE_NVPTX_H

#include <cstdint>
#include <string>

struct Vec3 {
  Vec3(int x, int y, int z) :
      x(x), y(y), z(z) {
  }
  int x;
  int y;
  int z;
};
using address_type = int8_t*;
extern "C" {
address_type __tlang_device_map(int kind, address_type address, uint64_t size);
void __tlang_device_sync(int id);
void __tlang_device_run_kernel(const void *fn, int id, Vec3 tensor_dim, Vec3 matrix_dim, void **args);
int __tlang_device_load_module(const std::string &filename);
void* __tlang_device_load_function(const std::string &functionname);
}

template <typename ...Args>
void __tlang_device_run_kernel_rt(const void *fn, int id, Vec3 tensor_dim, Vec3 matrix_dim, Args ...args) {
  void *kernel_args[] = { static_cast<void*>(&args)... };
  __tlang_device_run_kernel(fn, id, tensor_dim, matrix_dim, kernel_args);
}

#endif
