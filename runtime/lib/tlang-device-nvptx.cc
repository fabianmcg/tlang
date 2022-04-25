#include <tlang-device-nvptx.h>
#include <cuda_runtime.h>
#include <unordered_map>
#include <iostream>
#include <cstdlib>

#ifndef NDEBUG
#define CUDA_CHECK(status) { cuda_check((status), __FILE__, __LINE__); }
#else
#define CUDA_CHECK(status) { status; }
#endif
inline void cuda_check(cudaError_t status, const char *file, int line) {
  if (status != cudaSuccess) {
    std::cerr << "CUDA RT error: " << cudaGetErrorString(status) << "\n\t" << file << ":" << line << "" << std::endl;
    std::exit(status);
  }
}
namespace {
inline std::pair<dim3, dim3> getDefaultLaunchDims() {
  dim3 gsz(128), bsz(256);
  if (char *env = getenv("TLANG_NUM_TENSORS")) {
    int nt = atoi(env);
    if (nt > 0)
      gsz.x = nt;
  }
  if (char *env = getenv("TLANG_NUM_MATRIX")) {
    int nt = atoi(env);
    if (nt > 0)
      bsz.x = nt;
  }
  return {gsz, bsz};
}
struct TlangDeviceRuntime {
  typedef enum {
    to = 1,
    from = 2,
    create = 4,
    destroy = 8,
    present = 16
  } MapKind;
  TlangDeviceRuntime() :
      mapping(512) {
    CUDA_CHECK(cudaStreamCreate(&stream))
  }
  ~TlangDeviceRuntime() {
    for (auto [adress, mapped] : mapping)
      if (mapped)
        cudaFree(mapped);
    if (stream)
      cudaStreamDestroy(stream);
  }
  address_type map(MapKind kind, address_type address, uint64_t size) {
    if (!address)
      return nullptr;
    auto &mapped_address = mapping[address];
    if (kind == destroy) {
      if (mapped_address) {
        CUDA_CHECK(cudaFree(mapped_address))
        mapped_address = nullptr;
      }
      return nullptr;
    }
    if (kind == present) {
      if (mapped_address)
        return mapped_address;
      std::cerr << "Tlang RT error, address: " << address << " is not mapped to the device." << std::endl;
      std::exit(1);
    }
    if (mapped_address == nullptr)
      CUDA_CHECK(cudaMalloc((void** )&mapped_address, size))
    switch (kind) {
    case to:
      CUDA_CHECK(cudaMemcpyAsync(mapped_address, address, size, cudaMemcpyHostToDevice, stream))
      break;
    case from:
      CUDA_CHECK(cudaMemcpyAsync(address, mapped_address, size, cudaMemcpyDeviceToHost, stream))
      break;
    default:
      break;
    }
    return mapped_address;
  }
  inline void sync() {
    CUDA_CHECK(cudaStreamSynchronize(stream))
  }
  inline void launch(const void *fn, Vec3 tensor_dim, Vec3 matrix_dim, void **args) {
    CUDA_CHECK(
        cudaLaunchKernel(fn, dim3(tensor_dim.x, tensor_dim.y, tensor_dim.z), dim3(matrix_dim.x, matrix_dim.y, matrix_dim.z), args, 0,
            stream))
  }
  std::unordered_map<address_type, address_type> mapping;
  cudaStream_t stream { };
};
TlangDeviceRuntime runtime { };
}

extern "C" {
address_type __tlang_device_map(int kind, address_type address, uint64_t size) {
  return runtime.map(static_cast<TlangDeviceRuntime::MapKind>(kind), address, size);
}
void __tlang_device_sync(int id) {
  runtime.sync();
}
void __tlang_device_run_kernel(const void *fn, Vec3 tensor_dim, Vec3 matrix_dim, void **args) {
  runtime.launch(fn, tensor_dim, matrix_dim, args);
}
}
