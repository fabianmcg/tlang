#ifndef SUPPORT_VARIANT_HH
#define SUPPORT_VARIANT_HH

#include <variant>

namespace tlang {
template <typename ...Args>
struct Variant: public std::variant<Args...> {
  using std::variant<Args...>::variant;
  template <typename T>
  T* get() {
    return std::get_if<T>(this);
  }
  template <typename T>
  const T* get() const {
    return std::get_if<T>(this);
  }
  template <size_t I>
  std::variant_alternative_t<I, std::variant<Args...>>* get() {
    return std::get_if<I>(this);
  }
  template <size_t I>
  const std::variant_alternative_t<I, std::variant<Args...>>* get() const {
    return std::get_if<I>(this);
  }
};
}

#endif
