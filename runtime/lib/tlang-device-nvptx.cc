#include <tlang-device-nvptx.h>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <cuda_runtime.h>
#include <cuda.h>

//#ifndef NDEBUG
#define CUDA_CHECK(status) { cuda_rt_check((status), __FILE__, __LINE__); }
#define CUDA_DRIVER_CHECK(status) { cuda_driver_check((status), __FILE__, __LINE__); }
//#else
//#define CUDA_CHECK(status) { status; }
//#define CUDA_DRIVER_CHECK(status) { status; }
//#endif

namespace {
inline void cuda_rt_check(cudaError_t status, const char *file, int line) {
  if (status != cudaSuccess) {
    std::cerr << "CUDA RT error: " << cudaGetErrorString(status) << "\n\t" << file << ":" << line << "" << std::endl;
    std::exit(status);
  }
}
inline void cuda_driver_check(CUresult status, const char *file, int line) {
  if (status != CUDA_SUCCESS) {
    const char *errorString { };
    cuGetErrorString(status, &errorString);
    const char *errorName { };
    cuGetErrorName(status, &errorName);
    std::cerr << "CUDA Driver error: " << errorName << ": " << errorString << "\n\t" << file << ":" << line << "" << std::endl;
    std::exit(status);
  }
}

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

struct CUDAContext {
  CUcontext context { };
  CUdevice device { };
  CUresult status = CUDA_SUCCESS;
  CUDAContext() {
    init();
  }
  ~CUDAContext() {
    destroy();
  }
  int init() {
    CUDA_DRIVER_CHECK(cuInit(0));
    CUDA_DRIVER_CHECK(cuDeviceGet(&device, 0));
    CUDA_DRIVER_CHECK(cuCtxCreate(&context, 0, device));
    return 0;
  }
  inline void destroy() {
    if (context)
      CUDA_DRIVER_CHECK(cuCtxDestroy(context));
    context = nullptr;
  }
  void loadModule(const std::string &filename) {
    std::ifstream t(filename);
    if (!t.is_open()) {
      std::cerr << filename << " not found\n";
      std::exit(1);
    }
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    CUDA_DRIVER_CHECK(cuModuleLoadDataEx(&cudaModule, str.c_str(), 0, 0, 0));
  }
  CUfunction loadFunction(const std::string &functionName) {
    CUfunction function;
    CUDA_DRIVER_CHECK(cuModuleGetFunction(&function, cudaModule, functionName.c_str()));
    return function;
  }
  void launch(CUfunction function, Vec3 tensor_dim, Vec3 matrix_dim, void **arguments, cudaStream_t stream) {
    CUDA_DRIVER_CHECK(
        cuLaunchKernel(function, tensor_dim.x, tensor_dim.y, tensor_dim.z, matrix_dim.x, matrix_dim.y, matrix_dim.z, 0, stream, arguments, NULL));
  }
  CUmodule cudaModule { };
};
struct TlangDeviceRuntime {
  typedef enum {
    to = 1,
    from = 2,
    create = 4,
    destroy = 8,
    present = 16
  } MapKind;
  TlangDeviceRuntime() :
      mapping(512), context() {
    CUDA_CHECK(cudaStreamCreate(&stream))
    run_stream = stream;
  }
  ~TlangDeviceRuntime() {
    for (auto [adress, mapped] : mapping)
      if (mapped)
        cudaFree(mapped);
    if (stream)
      cudaStreamDestroy(stream);
  }
  void loadModule(const std::string &filename) {
    context.loadModule(filename);
  }
  void* loadFunction(const std::string &functionName) {
    return context.loadFunction(functionName);
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
    context.launch(static_cast<CUfunction>(const_cast<void*>(fn)), tensor_dim, matrix_dim, args, run_stream);
  }
  inline void set_stream(cudaStream_t stream) {
    run_stream = stream;
  }
  std::unordered_map<address_type, address_type> mapping;
  cudaStream_t stream { };
  cudaStream_t run_stream { };
  CUDAContext context { };
};
std::unique_ptr<TlangDeviceRuntime> runtime { };
inline TlangDeviceRuntime& getRuntime() {
  if (!runtime)
    runtime = std::unique_ptr<TlangDeviceRuntime> { new TlangDeviceRuntime() };
  return *runtime;
}
}

extern "C" {
int __tlang_device_init() {
  getRuntime();
  return 0;
}
address_type __tlang_device_map(int kind, address_type address, uint64_t size) {
  return getRuntime().map(static_cast<TlangDeviceRuntime::MapKind>(kind), address, size);
}
void __tlang_device_sync(int id) {
  getRuntime().sync();
}
void __tlang_device_set_stream(void *stream) {
  getRuntime().set_stream((cudaStream_t) stream);
}
void __tlang_device_run_kernel(const void *fn, int id, Vec3 tensor_dim, Vec3 matrix_dim, void **args) {
  getRuntime().launch(fn, tensor_dim, matrix_dim, args);
}
int __tlang_device_load_module(const std::string &filename) {
  getRuntime().loadModule(filename);
  return 0;
}
void* __tlang_device_load_function(const std::string &functionname) {
  return getRuntime().loadFunction(functionname);
}
}
