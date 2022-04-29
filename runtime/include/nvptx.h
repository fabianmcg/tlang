#ifndef RUNTIME_NVPTX_H
#define RUNTIME_NVPTX_H

#ifdef __cplusplus
#include <cstdint>
#include <cstddef>
#else
#include <stdint.h>
#include <stddef.h>
#endif

typedef struct tlang_nvptx_module *tlang_nvptx_mod;
typedef struct tlang_nvptx_function *tlang_nvptx_fn;
typedef struct tlang_nvptx_stream *tlang_nvptx_stream_t;

typedef enum {
  TLANG_MEMCPY_AUTO,
  TLANG_MEMCPY_D2H,
  TLANG_MEMCPY_H2D,
  TLANG_MEMCPY_D2D,
  TLANG_MEMCPY_H2H,
} TlangNVPTXMemcpyKind;


typedef struct {
  unsigned x;
  unsigned y;
  unsigned z;
} tlang_vec3;

typedef struct {
  void **arguments;
  tlang_vec3 gridSize;
  tlang_vec3 blockSize;
  tlang_nvptx_stream_t stream;
  uint32_t sharedMemSize;
} tlang_kernel_parameters;

#ifdef __cplusplus
extern "C" {
#endif

int tlang_nvptx_synchronize();

int tlang_nvptx_status();

void* tlang_nvptx_malloc(size_t size);

void tlang_nvptx_free(void*);

#ifdef __cplusplus
void tlang_nvptx_memcpy(TlangNVPTXMemcpyKind kind, void *dest, void *src, size_t size, tlang_nvptx_stream_t stream = nullptr);
#else
void tlang_nvptx_memcpy(TlangNVPTXMemcpyKind kind, void *dest, void *src, size_t size, tlang_nvptx_stream_t stream);
#endif

tlang_nvptx_mod tlang_nvptx_get_module(const char *filename);

tlang_nvptx_fn tlang_nvptx_get_function(tlang_nvptx_mod module, const char *kernelName);

#ifdef __cplusplus
tlang_kernel_parameters tlang_nvptx_kernel_parameters(void **arguments, tlang_vec3 gridSize, tlang_vec3 blockSize,
    tlang_nvptx_stream_t stream = nullptr, uint32_t sharedMemSize = 0);
#else
tlang_kernel_parameters tlang_nvptx_kernel_parameters(void **arguments, tlang_vec3 gridSize, tlang_vec3 blockSize,
    tlang_nvptx_stream_t stream, uint32_t sharedMemSize);
#endif

int tlang_nvptx_kernel_launch(tlang_nvptx_fn function, tlang_kernel_parameters parameters);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
template <typename T>
T* tlang_nvptx_malloc(size_t size) {
  return reinterpret_cast<T*>(tlang_nvptx_malloc(size * sizeof(T)));
}
template <size_t N>
struct __tlang_nvptx_kernel_args {
  void *arguments[N];
  operator void**() {
    return arguments;
  }
};
template <>
struct __tlang_nvptx_kernel_args<0> {
  operator void**() {
    return nullptr;
  }
};
template <typename ...Args>
__tlang_nvptx_kernel_args<sizeof...(Args)> __tlang_device_argument_list(Args &...args) {
  return __tlang_nvptx_kernel_args<sizeof...(Args)> { &args... };
}
#endif
#endif
