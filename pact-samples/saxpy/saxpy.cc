#include <iostream>
#include <vector>
#include "util.hh"

#ifndef CUDA_VER
#include <tlang-device-nvptx.h>
#else
#include <cuda_runtime.h>
#endif

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
  std::cerr << "SET:  " << et << std::endl;
  std::cerr << "SETI: " << timer::pretty_str(et.elapsed() / its) << std::endl;
  return et.elapsed_time();
}

#ifndef CUDA_VER
extern "C" void add_tlang(double alpha, double *x, double *y, int n);
double gpu(double alpha, std::vector<double> &x, std::vector<double> &y, size_t its) {
  auto et = timeit([&]() {
    for (size_t i = 0; i < its; ++i)
      add_tlang(alpha, x.data(), y.data(), x.size());
    __tlang_device_sync(0);
  });
  std::cerr << "TET:  " << et << std::endl;
  std::cerr << "TETI: " << timer::pretty_str(et.elapsed() / its) << std::endl;
  return et.elapsed_time();
}
#else
#include <stdio.h>
#ifndef NDEBUG
#define CHECK_CUDA(ans) { gpuAssert((ans), __FILE__, __LINE__); }
#else
#define CHECK_CUDA(ans) { ans; }
#endif
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
  CHECK_CUDA(cudaMalloc(&xg, sizeof(double) * x.size()));
  CHECK_CUDA(cudaMalloc(&yg, sizeof(double) * x.size()));

  int size = x.size();
  int bs = 256;
  unsigned gs = (size + bs - 1) / bs;
  CHECK_CUDA(cudaDeviceSynchronize());
  auto et = timeit([&]() {
    for (size_t i = 0; i < its; ++i) {
      CHECK_CUDA(cudaMemcpy((void*) xg, (void*) x.data(), x.size() * sizeof(double), cudaMemcpyHostToDevice));
      CHECK_CUDA(cudaMemcpy((void*) yg, (void*) y.data(), y.size() * sizeof(double), cudaMemcpyHostToDevice));
      saxpy_kernel<<<gs, bs>>>(alpha, xg, yg, size, bs);
      CHECK_CUDA(cudaMemcpy((void*) x.data(), (void*) xg, x.size() * sizeof(double), cudaMemcpyDeviceToHost));
    }
    cudaDeviceSynchronize();
  });
  CHECK_CUDA(cudaDeviceSynchronize());
  CHECK_CUDA(cudaDeviceSynchronize());
  CHECK_CUDA(cudaFree(xg));
  CHECK_CUDA(cudaFree(yg));
  std::cerr << "CUDA total elapsed time:  " << et << std::endl;
  std::cerr << "CUDA elapsed time per it: " << timer::pretty_str(et.elapsed() / its) << std::endl;
  return et.elapsed_time();
}
#endif

int main() {
  size_t size { 1024 * 1024 }, its = 10;
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
