#ifndef CUDA_VER
#include <nvptx.h>
#else
#include <cuda_runtime.h>
#endif
#include <iostream>
#include "util.hh"

template <typename T>
void saxpy(T alpha, std::vector<T> &x, const std::vector<T> &y) {
  for (size_t i = 0; i < x.size(); ++i)
    x[i] = alpha * x[i] + y[i];
}

double serial(double alpha, std::vector<double> &x, std::vector<double> &y, size_t its) {
  auto et = timeit([&]() {
    for (size_t i = 0; i < its; ++i)
      saxpy(alpha, x, y);
  });
  std::cerr << "Serial total elapsed time:  " << et << std::endl;
  std::cerr << "Serial elapsed time per it: " << timer::pretty_str(et.elapsed() / its) << std::endl;
  return et.elapsed_time();
}
#ifndef CUDA_VER
double gpu(double alpha, std::vector<double> &x, std::vector<double> &y, size_t its) {
  auto module = tlang_nvptx_get_module("add.ptx");
  auto kernel = tlang_nvptx_get_function(module, "nvptx_kernel");
  double *xg = tlang_nvptx_malloc<double>(x.size());
  double *yg = tlang_nvptx_malloc<double>(y.size());
  tlang_nvptx_memcpy(TLANG_MEMCPY_H2D, xg, x.data(), x.size() * sizeof(double), nullptr);
  tlang_nvptx_memcpy(TLANG_MEMCPY_H2D, yg, y.data(), y.size() * sizeof(double), nullptr);
  int size = x.size();
  int bs = 256;
  unsigned gs = (size + bs - 1) / bs;
  auto arguments = tlang_nvptx_argument_list(alpha, xg, yg, size, bs);
  auto parameters = tlang_nvptx_kernel_parameters(arguments.arguments, { gs, 1, 1 }, { (unsigned)bs, 1, 1 }, nullptr, 0);
  tlang_nvptx_synchronize();
  auto et = timeit([&]() {
    for (size_t i = 0; i < its; ++i)
      tlang_nvptx_kernel_launch(kernel, parameters);
    tlang_nvptx_synchronize();
  });
  tlang_nvptx_memcpy(TLANG_MEMCPY_D2H, x.data(), xg, x.size() * sizeof(double), nullptr);
  tlang_nvptx_free(xg);
  tlang_nvptx_free(yg);
  std::cerr << "Tlang total elapsed time:  " << et << std::endl;
  std::cerr << "Tlang elapsed time per it: " << timer::pretty_str(et.elapsed() / its) << std::endl;
  return et.elapsed_time();
}
#else
#include <stdio.h>
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort = true) {
  if (code != cudaSuccess) {
    fprintf(stderr, "GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
    if (abort)
      exit(code);
  }
}
__global__ void saxpy_kernel(double alpha, double *x, const double *__restrict__ y, int n, int bs) {
  int i = threadIdx.x + bs * blockIdx.x;
  if (i < n)
    x[i] = alpha * x[i] + y[i];
}
double gpu(double alpha, std::vector<double> &x, std::vector<double> &y, size_t its) {
  double *xg, *yg;
  gpuErrchk(cudaMalloc(&xg, sizeof(double) * x.size()));
  gpuErrchk(cudaMalloc(&yg, sizeof(double) * x.size()));
  gpuErrchk(cudaMemcpy((void*) xg, (void*) x.data(), x.size() * sizeof(double), cudaMemcpyHostToDevice));
  gpuErrchk(cudaMemcpy((void*) yg, (void*) y.data(), y.size() * sizeof(double), cudaMemcpyHostToDevice));

  int size = x.size();
  int bs = 256;
  unsigned gs = (size + bs - 1) / bs;
  gpuErrchk(cudaDeviceSynchronize());
  auto et = timeit([&]() {
    for (size_t i = 0; i < its; ++i) {
      saxpy_kernel<<<gs, bs>>>(alpha, xg, yg, size, bs);
    }
    cudaDeviceSynchronize();
  });
  gpuErrchk(cudaDeviceSynchronize());
  gpuErrchk(cudaMemcpy((void*) x.data(), (void*) xg, x.size() * sizeof(double), cudaMemcpyDeviceToHost));
  gpuErrchk(cudaDeviceSynchronize());
  gpuErrchk(cudaFree(xg));
  gpuErrchk(cudaFree(yg));
  std::cerr << "CUDA total elapsed time:  " << et << std::endl;
  std::cerr << "CUDA elapsed time per it: " << timer::pretty_str(et.elapsed() / its) << std::endl;
  return et.elapsed_time();
}
#endif

int main() {
  size_t size { 1024 * 1024 * 64 }, its = 1;
  std::vector<double> x = random_vector<double>(size, 0., 1.), y = random_vector<double>(size, 0., 1.);
  std::vector<double> xg = x, yg = y;
  double alpha = random<double>(1., 2.);
  double gt { };
  auto ct = serial(alpha, x, y, its);
  gt = gpu(alpha, xg, yg, its);
//  print(std::cerr, x) << std::endl;
//  print(std::cerr, xg) << std::endl;
  std::cerr << "Speedup: " << (ct / gt) << std::endl;
  auto equal = numeric_equal(x, xg, false, 1e-10);
  std::cerr << "error(x, xg) < 1e-10: " << std::boolalpha << equal << std::endl;
  return 0;
}
