#ifndef __COMMON_STATIC_FOR_HH__
#define __COMMON_STATIC_FOR_HH__

#include <utility>
#include <tuple>
#include <type_traits>

namespace tlang {
template <int begin, int end, bool reversed = false>
struct static_for {
  static_assert(begin < end);
  template <typename T>
  inline void operator()(T &&function) {
    if constexpr (reversed) {
      function.template execute<end - 1>();
      static_for<begin, end - 1, reversed> sf;
      sf(std::forward<T>(function));
    } else {
      function.template execute<begin>();
      static_for<begin + 1, end, reversed> sf;
      sf(std::forward<T>(function));
    }
  }
};
template <int begin, bool reversed>
struct static_for<begin, begin, reversed> {
  template <typename T = void>
  inline void operator()(T &&function) {
  }
};

template <typename TT, typename FT>
struct visit_tuple_helper {
  TT &tuple;
  FT &function;
  template <int it>
  void execute() {
    function.template operator()<it>(std::get<it>(tuple));
  }
};
template <bool reversed = false, typename TT = void, typename FT = void>
void visit_tuple(TT &&tuple, FT &&function) {
  constexpr size_t size = std::tuple_size_v<std::remove_reference_t<TT>>;
  visit_tuple_helper<TT, FT> visitor { tuple, function };
  static_for<0, size, reversed> sf { };
  sf(visitor);
}
}
#endif
