#ifndef EXAMPLES_UTIL_HH
#define EXAMPLES_UTIL_HH

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <time.h>
#include <vector>

namespace clock_ns {
inline std::string frmt(std::chrono::nanoseconds ns) {
  using namespace std;
  using namespace std::chrono;
  std::ostringstream os;
  char fill = os.fill();
  os.fill('0');
  auto m = duration_cast<minutes>(ns);
  ns -= m;
  auto s = duration_cast<seconds>(ns);
  ns -= s;
  os << setw(2) << m.count() << ":" << setw(2) << s.count() << "." << setw(9) << ns.count();
  os.fill(fill);
  return os.str();
}
inline double elapsed_time(std::chrono::nanoseconds ns) {
  return std::chrono::duration_cast<std::chrono::duration<double>>(ns).count();
}
inline std::chrono::nanoseconds now(clockid_t type) {
  timespec tm;
  clock_gettime(type, &tm);
  std::chrono::nanoseconds tp = std::chrono::seconds(tm.tv_sec) + std::chrono::nanoseconds(tm.tv_nsec);
  return tp;
}
}
struct timer {
  std::chrono::nanoseconds t1 = std::chrono::nanoseconds::zero();
  std::chrono::nanoseconds t2 = std::chrono::nanoseconds::zero();
  timer() = default;
  ~timer() = default;
  timer(timer&&) = default;
  timer(const timer&) = default;
  timer& operator=(timer&&) = default;
  timer& operator=(const timer&) = default;
  inline std::chrono::nanoseconds elapsed() const {
    return t2 - t1;
  }
  inline double elapsed_time() const {
    return clock_ns::elapsed_time(t2 - t1);
  }
  inline std::string pretty_str() const {
    return clock_ns::frmt(elapsed());
  }
  static inline std::string pretty_str(std::chrono::nanoseconds ns) {
    return clock_ns::frmt(ns);
  }
};
struct cpu_timer: timer {
  using timer::timer;
  clockid_t type = CLOCK_MONOTONIC;
  inline void start() {
    t1 = clock_ns::now(type);
  }
  inline void stop() {
    t2 = clock_ns::now(type);
  }
};

inline std::ostream& operator<<(std::ostream &oss, const timer &timer) {
  oss << timer.pretty_str();
  return oss;
}

template <typename F, typename ...Args>
double ctimeit(size_t it, F &&function, Args &&...args) {
  cpu_timer timer { };
  timer.start();
  for (size_t i = 0; i < it; ++i)
    function(std::forward<Args>(args)...);
  timer.stop();
  double div = it > 0 ? it : 1;
  return timer.elapsed_time() / div;
}

template <typename F, typename ...Args>
cpu_timer timeit(F &&function, Args &&...args) {
  cpu_timer timer { };
  timer.start();
  function(std::forward<Args>(args)...);
  timer.stop();
  return timer;
}

template <typename T>
std::ostream& print(std::ostream &ost, const std::vector<T> &v) {
  auto f = ost.flags();
  ost << std::fixed;
  ost << std::setprecision(6);
  ost << "{";
  for (size_t i = 0; i < v.size(); ++i) {
    ost << v[i];
    if ((i + 1) < v.size())
      ost << ", ";
  }
  ost << "}";
  ost.flags(f);
  return ost;
}

template <typename T>
void random_real(T *array, size_t n, T a = 0., T b = 1.) {
  std::default_random_engine generator;
  std::uniform_real_distribution<T> distribution(a, b);
#pragma omp parallel
  for (size_t i = 0; i < n; ++i)
    array[i] = distribution(generator);
}
template <typename T>
void random_int(T *array, size_t n, T a = 0, T b = 10) {
  std::default_random_engine generator;
  std::uniform_int_distribution<T> distribution(a, b);
#pragma omp parallel
  for (size_t i = 0; i < n; ++i)
    array[i] = distribution(generator);
}
template <typename T>
inline void random(T *array, size_t n, T a, T b) {
  if constexpr (std::is_integral_v<T>)
    random_int(array, n, a, b);
  else if constexpr (std::is_floating_point_v<T>)
    random_real(array, n, a, b);
}
template <typename T>
inline T random(T a, T b) {
  T value;
  if constexpr (std::is_integral_v<T>)
    random_int(&value, 1, a, b);
  else if constexpr (std::is_floating_point_v<T>)
    random_real(&value, 1, a, b);
  return value;
}
template <typename T>
bool equal(const std::vector<T> &A, const std::vector<T> &B, bool ignore_size_check = false) {
  if (B.size() == A.size() || ignore_size_check) {
    bool equal = true;
    size_t sz = std::min(A.size(), B.size());
#pragma omp parallel for
    for (size_t i = 0; i < sz; ++i)
      if (A[i] != B[i])
        equal = false;
    return equal;
  }
  return false;
}
template <typename T>
bool numeric_equal(const std::vector<T> &A, const std::vector<T> &B, bool ignore_size_check = false, T eps = 1e-6) {
  if (B.size() == A.size() || ignore_size_check) {
    bool equal = true;
    size_t sz = std::min(A.size(), B.size());
#pragma omp parallel for
    for (size_t i = 0; i < sz; ++i)
      if (std::abs(A[i] - B[i]) > eps)
        equal = false;
    return equal;
  }
  return false;
}
template <typename T>
std::vector<T> random_vector(size_t n, T a, T b) {
  std::vector<T> A(n);
  random(A.data(), n, a, b);
  return A;
}

#endif
