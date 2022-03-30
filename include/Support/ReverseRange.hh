#ifndef SUPPORT_REVERSERANGE_HH
#define SUPPORT_REVERSERANGE_HH

namespace tlang {
template <typename T>
struct ReverseRange {
public:
  ReverseRange(T &container) :
      container(container) {
  }
  auto begin() {
    return container.rbegin();
  }
  auto begin() const {
    return container.rbegin();
  }
  auto end() {
    return container.rend();
  }
  auto end() const {
    return container.rend();
  }
private:
  T &container;
};
template <typename T>
ReverseRange<T> make_reverse(T &container) {
  return ReverseRange<T> { container };
}
template <typename T>
ReverseRange<const T> make_reverse(const T &container) {
  return ReverseRange<const T> { container };
}
}

#endif
