#ifndef SUPPORT_ENUMERATE_HH
#define SUPPORT_ENUMERATE_HH

#include <iterator>

template <typename Iterator>
class Enumerator {
public:
  class EnumerateIterator {
  private:
    size_t idx { };
    Iterator it;
  public:
    using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using value_type = std::pair<size_t, typename std::iterator_traits<Iterator>::value_type>;
    using pointer = std::pair<size_t, typename std::iterator_traits<Iterator>::pointer>;
    using reference = std::pair<size_t, typename std::iterator_traits<Iterator>::reference>;
    EnumerateIterator(Iterator it) :
        it(it) {
    }
    EnumerateIterator& operator++() {
      ++it;
      ++idx;
      return *this;
    }
    EnumerateIterator operator++(int) {
      EnumerateIterator retval = *this;
      ++(*this);
      return retval;
    }
    bool operator==(EnumerateIterator other) const {
      return it == other.it;
    }
    bool operator!=(EnumerateIterator other) const {
      return !(*this == other);
    }
    auto operator*() const -> std::pair<std::size_t, decltype(*it)> {
      return {idx, *it};
    }
  };
  Enumerator(Iterator begin, Iterator end) :
      b(begin), e(end) {
  }
  EnumerateIterator begin() {
    return b;
  }
  EnumerateIterator end() {
    return e;
  }
private:
  Iterator b;
  Iterator e;
};
template <typename T>
Enumerator<typename T::iterator> enumerate(T &container) {
  return Enumerator<typename T::iterator>(container.begin(), container.end());
}
template <typename T>
Enumerator<typename T::const_iterator> enumerate(const T &container) {
  return Enumerator<typename T::const_iterator>(container.cbegin(), container.cend());
}
#endif
