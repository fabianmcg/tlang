#include <iostream>
#include <vector>
#include "util.hh"

#ifndef OMP_VER
#include <tlang-device-nvptx.h>
#else
#include <omp.h>
#endif

template <typename T>
T dot(std::vector<T> &x, const std::vector<T> &y) {
  T result {};
  for (size_t i = 0; i < x.size(); ++i)
    result += x[i] * y[i];
  return result;
}

double serial(double &result, std::vector<double> &x, std::vector<double> &y, size_t its) {
  auto et = timeit([&]() {
    for (size_t i = 0; i < its; ++i)
      result = dot(x, y);
  });
  std::cerr << "SET:  " << et << std::endl;
  std::cerr << "SETI: " << timer::pretty_str(et.elapsed() / its) << std::endl;
  return et.elapsed_time();
}

#ifndef OMP_VER
extern "C" double dot_tlang(double *x, double *y, int n);
double gpu(double &result, std::vector<double> &x, std::vector<double> &y, size_t its) {
  auto et = timeit([&]() {
    for (size_t i = 0; i < its; ++i)
      result = dot_tlang(alpha, x.data(), y.data(), x.size());
    __tlang_device_sync(0);
  });
  std::cerr << "TET:  " << et << std::endl;
  std::cerr << "TETI: " << timer::pretty_str(et.elapsed() / its) << std::endl;
  return et.elapsed_time();
}
#else
template <typename T>
T dot_mp(std::vector<T> &x, const std::vector<T> &y) {
  T result {};
#pragma omp target teams loop reduce(+, result)
  for (size_t i = 0; i < x.size(); ++i)
    result += x[i] * y[i];
  return result;
}
double gpu(double &result, std::vector<double> &x, std::vector<double> &y, size_t its) {
  auto et = timeit([&]() {
    for (size_t i = 0; i < its; ++i)
      result = dot_mp(x, y);
  });
  std::cerr << "TET:  " << et << std::endl;
  std::cerr << "TETI: " << timer::pretty_str(et.elapsed() / its) << std::endl;
  return et.elapsed_time();
}
#endif

int main() {
  size_t size { 1024 * 1024 }, its = 10;
  std::vector<double> x = random_vector<double>(size, 0., 1.), y = random_vector<double>(size, 0., 1.);
  double rc{}, rg{};
  auto ct = serial(rc, x, y, its);
  auto gt = gpu(rg, xg, yg, its);
  std::cerr << "Speedup: " << (ct / gt) << std::endl;
  auto equal = std::abs(rc - rg) < 1e-10;
  std::cerr << "error(x, xg) < 1e-10: " << std::boolalpha << equal << std::endl;
  return 0;
}
