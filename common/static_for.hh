#ifndef COMMON_STATIC_FOR_HH_
#define COMMON_STATIC_FOR_HH_

#include <utility>
#include <type_traits>

template <int begin, int end>
struct static_for {
  static_assert(begin < end);
  template <typename T>
  inline void operator()(T &&function) {
    function.template execute<begin>();
    static_for<begin + 1, end> sf;
    sf(std::forward<T>(function));
  }
};
template <int begin>
struct static_for<begin, begin> {
  template <typename T>
  inline void operator()(T &&function) {
  }
};
#endif
