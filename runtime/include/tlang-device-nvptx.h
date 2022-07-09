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
using address_type = void*;
extern "C" {
int __tlang_device_init();
address_type __tlang_device_map(int kind, address_type address, uint64_t size);
void __tlang_device_sync(int id);
void __tlang_device_run_kernel(const void *fn, int id, Vec3 tensor_dim, Vec3 matrix_dim, void **args);
void __tlang_device_set_stream(void *stream);
int __tlang_device_load_module(const std::string &filename);
void* __tlang_device_load_function(const std::string &functionname);
}

template <size_t N>
struct __tlang_device_kernel_args {
  void *arguments[N];
  operator void**() {
    return arguments;
  }
};
template <>
struct __tlang_device_kernel_args<0> {
  operator void**() {
    return nullptr;
  }
};
template <typename ...Args>
__tlang_device_kernel_args<sizeof...(Args)> __tlang_device_argument_list(Args &...args) {
  return __tlang_device_kernel_args<sizeof...(Args)> { &args... };
}
template <typename ...Args>
void __tlang_device_run_kernel_rt(const void *fn, int id, Vec3 tensor_dim, Vec3 matrix_dim, Args ...args) {
  auto kernel_args = __tlang_device_argument_list(args...);
  __tlang_device_run_kernel(fn, id, tensor_dim, matrix_dim, kernel_args);
}

#endif
