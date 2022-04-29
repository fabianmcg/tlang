#ifndef BASIC_INFRA_PARENTLIST_HH
#define BASIC_INFRA_PARENTLIST_HH

#include <cstddef>
#include <tuple>

namespace tlang {
template <typename ...T>
struct ParentList {
  static constexpr size_t size = sizeof...(T);
  using parent_list = std::tuple<T...>;
  template <int I>
  using parent_t = std::tuple_element_t<I, parent_list>;
};
template <>
struct ParentList<> {
  static constexpr size_t size = 0;
};
}

#endif
