#ifndef TLANG_RUNTIME_HOST_H
#define TLANG_RUNTIME_HOST_H

#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>

extern "C" {
struct __tlang_host_arguments {
  int tid;
  void *data;
};
void __tlang_host_init(int);
void __tlang_host_exit();
void __tlang_host_create_parallel(int num_tensors, void* (*exec)(void*), void *data);
void __tlang_host_sync();
int __tlang_host_tid();
int __tlang_host_dim();
}
template <typename ...Arguments>
class __tlang_host_kernel_arguments {
public:
  inline __tlang_host_kernel_arguments(void *function, Arguments ...args) :
      function(function), arguments(args...) {
  }
  inline void* run() {
    auto kernel = reinterpret_cast<void (*)(std::remove_reference_t<Arguments>...)>(function);
    std::apply(kernel, arguments);
    return nullptr;
  }
protected:
  void *function;
  std::tuple<Arguments...> arguments;
};
template <>
class __tlang_host_kernel_arguments<> {
public:
  inline __tlang_host_kernel_arguments(void *function) :
      function(function) {
  }
  inline void* run() {
    auto kernel = reinterpret_cast<void (*)()>(function);
    kernel();
    __tlang_host_exit();
    return nullptr;
  }
protected:
  void *function;
};
template <typename ...Arguments>
inline void __tlang_host_run_kernel_rt(int num_tensors, void *fn, Arguments ...arguments) {
  auto kernel_object = __tlang_host_kernel_arguments<Arguments...>(fn, arguments...);
  auto stub = [](void *stub_arguments) -> void* {
    auto object = reinterpret_cast<__tlang_host_arguments*>(stub_arguments);
    __tlang_host_init(object->tid);
    return reinterpret_cast<__tlang_host_kernel_arguments <Arguments...>*>(object->data)->run();
  };
  __tlang_host_create_parallel(num_tensors, stub, &kernel_object);
}
#endif
