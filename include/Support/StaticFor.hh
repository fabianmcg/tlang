#ifndef SUPPORT_STATICFOR_HH
#define SUPPORT_STATICFOR_HH

#include <tuple>
#include <type_traits>

namespace tlang {
template <int begin, int end, bool reversed = false>
struct static_for {
  static_assert(begin < end);
  template <typename F>
  static inline void execute(F &&function) {
    if constexpr (reversed) {
      function.template execute<end - 1>();
      static_for<begin, end - 1, reversed> { }.execute(std::forward<F>(function));
    } else {
      function.template execute<begin>();
      static_for<begin + 1, end, reversed> { }.execute(std::forward<F>(function));
    }
  }
  template <typename F>
  inline void operator()(F &&function) const {
    execute(std::forward<F>(function));
  }
};
template <int it, bool reversed>
struct static_for<it, it, reversed> {
  template <typename F>
  static inline void execute(F &&function) {
  }
  template <typename T>
  inline void operator()(T &&function) const {
  }
};
namespace impl {
template <typename Tuple, typename Function>
struct VisitTuple {
  Tuple &tuple;
  Function &function;
  template <int it>
  void execute() {
    function.template execute<it>(std::get<it>(tuple));
  }
};
}
template <bool reversed = false, typename Tuple = void, typename Function = void>
void visit_tuple(Tuple &&tuple, Function &&function) {
  impl::VisitTuple<Tuple, Function> visitor { tuple, function };
  static_for<0, std::tuple_size_v<std::remove_reference_t<Tuple>>, reversed> { }.execute(visitor);
}
}
#endif
