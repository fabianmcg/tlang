#ifndef __COMMON_UTILITY_HH__
#define __COMMON_UTILITY_HH__

namespace __lang_np__ {
template <typename ...Args>
bool all(Args ... args) {
  return (... && args);
}
template <typename ... Args>
bool any(Args ... args) {
  return (... || args);
}
template <typename T, typename ... Args>
bool is_any(T _0, Args ... args) {
  if constexpr (sizeof...(Args))
      return any((_0 == args) ...);
  return false;
}
}
#endif
