#include "nvptx.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <list>
#include <memory>
#include <cuda.h>

struct tlang_nvptx_module {
  CUmodule cudaModule;
};
struct tlang_nvptx_function {
  CUfunction function;
};
struct tlang_nvptx_stream {

};

namespace {
struct NVPTXContext {
  CUcontext context { };
  CUdevice device { };
  CUresult status = CUDA_SUCCESS;
  NVPTXContext() {
    init();
  }
  ~NVPTXContext() {
    destroy();
  }
  int init() {
    check(cuInit(0));
    check(cuDeviceGet(&device, 0));
    check(cuCtxCreate(&context, 0, device));
    return 0;
  }
  inline void check(CUresult status, bool emmitAssert = true) {
    if (emmitAssert)
      assert(status == CUDA_SUCCESS);
    this->status = status;
  }
  inline void destroy() {
    if (context)
      check(cuCtxDestroy(context), false);
    context = nullptr;
  }
  inline void* alloc(unsigned long int size) {
    CUdeviceptr ptr;
    check(cuMemAlloc(&ptr, size));
    return reinterpret_cast<void*>(ptr);
  }
  inline void free(void *iptr) {
    if (iptr) {
      CUdeviceptr ptr = reinterpret_cast<CUdeviceptr>(iptr);
      check(cuMemFree(ptr));
    }
  }
  void memcpy(TlangNVPTXMemcpyKind kind, void *dest, void *src, size_t size) {
    switch (kind) {
    case TLANG_MEMCPY_H2D: {
      CUdeviceptr devPtr = reinterpret_cast<CUdeviceptr>(dest);
      check(cuMemcpyHtoD(devPtr, src, size));
      break;
    }
    case TLANG_MEMCPY_D2H: {
      CUdeviceptr devPtr = reinterpret_cast<CUdeviceptr>(src);
      check(cuMemcpyDtoH(dest, devPtr, size));
      break;
    }
    default:
      break;
    }
  }
  tlang_nvptx_mod addModule(const std::string &filename) {
    std::ifstream t(filename);
    if (!t.is_open()) {
      std::cerr << filename << " not found\n";
      std::exit(1);
    }
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    CUmodule cudaModule;
    check(cuModuleLoadDataEx(&cudaModule, str.c_str(), 0, 0, 0));
    modules.push_back(std::unique_ptr<tlang_nvptx_module>(new tlang_nvptx_module { cudaModule }));
    return modules.back().get();
  }
  tlang_nvptx_fn addFunction(tlang_nvptx_module module, const std::string &functionName) {
    CUfunction function;
    check(cuModuleGetFunction(&function, module.cudaModule, functionName.c_str()));
    functions.push_back(std::unique_ptr<tlang_nvptx_function>(new tlang_nvptx_function { function }));
    return functions.back().get();
  }
  int sync() {
    check(cuCtxSynchronize());
    return status;
  }
  std::list<std::unique_ptr<tlang_nvptx_module>> modules;
  std::list<std::unique_ptr<tlang_nvptx_function>> functions;
};
NVPTXContext nvptx_context { };
}

int tlang_nvptx_synchronize() {
  return nvptx_context.sync();
}

int tlang_nvptx_status() {
  return nvptx_context.status;
}

void* tlang_nvptx_malloc(size_t size) {
  return nvptx_context.alloc(size);
}

void tlang_nvptx_free(void *ptr) {
  nvptx_context.free(ptr);
}

void tlang_nvptx_memcpy(TlangNVPTXMemcpyKind kind, void *dest, void *src, size_t size, tlang_nvptx_stream_t stream) {
  nvptx_context.memcpy(kind, dest, src, size);
}

tlang_nvptx_mod tlang_nvptx_get_module(const char *filename) {
  return nvptx_context.addModule(filename);
}

tlang_nvptx_fn tlang_nvptx_get_function(tlang_nvptx_mod module, const char *kernelName) {
  if (module)
    return nvptx_context.addFunction(*module, kernelName);
  return nullptr;
}

tlang_kernel_parameters tlang_nvptx_kernel_parameters(void **arguments, tlang_vec3 gridSize, tlang_vec3 blockSize,
    tlang_nvptx_stream_t stream, uint32_t sharedMemSize) {
  return tlang_kernel_parameters { arguments, gridSize, blockSize, stream, sharedMemSize };
}

int tlang_nvptx_kernel_launch(tlang_nvptx_fn function, tlang_kernel_parameters parameters) {
  nvptx_context.check(
      cuLaunchKernel(function->function, parameters.gridSize.x, parameters.gridSize.y, parameters.gridSize.z, parameters.blockSize.x,
          parameters.blockSize.y, parameters.blockSize.z, 0, NULL, parameters.arguments, NULL));
  return nvptx_context.status;
}
