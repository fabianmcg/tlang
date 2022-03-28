#ifndef AST_INFRA_PARENTLIST_HH
#define AST_INFRA_PARENTLIST_HH

#include <tuple>

namespace tlang {
template <typename ...T>
struct ParentClasses {
  static constexpr size_t size = sizeof...(T);
  using parent_types_t = std::tuple<T...>;
  template <int I>
  using type_t = std::tuple_element_t<I, parent_types_t>;
};
template <>
struct ParentClasses<> {
  static constexpr size_t size = 0;
  template <int I>
  using type_t = void;
};
}

#endif
