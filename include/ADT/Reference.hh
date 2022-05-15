#ifndef __COMMON_REFERENCE_HH__
#define __COMMON_REFERENCE_HH__

namespace tlang {
template <typename T>
struct reference {
  using type = T;
  type *__data { };
  reference() = default;
  ~reference() = default;
  reference(reference&&) = default;
  reference(const reference&) = default;
  inline reference(type &value) {
    __data = &value;
  }
  inline reference(type *value) {
    __data = value;
  }
  inline operator type*() const {
    return __data;
  }
  reference& operator=(reference&&) = default;
  reference& operator=(const reference&) = default;
  inline reference& operator=(type &value) {
    __data = &value;
    return *this;
  }
  inline reference& operator=(type *value) {
    __data = value;
    return *this;
  }
  template <typename V>
  inline reference& operator=(V &value) {
    __data = static_cast<type*>(&value);
    return *this;
  }
  template <typename V>
  inline reference& operator=(V *value) {
    __data = static_cast<type*>(value);
    return *this;
  }
  inline operator bool() const {
    return __data;
  }
  inline type& operator*() {
    return *__data;
  }
  inline const type& operator*() const {
    return *__data;
  }
  type* operator->() {
    return __data;
  }
  const type* operator->() const {
    return __data;
  }
  inline type*& data() {
    return __data;
  }
  inline const type*& data() const {
    return __data;
  }
  inline type& ref() {
    return *__data;
  }
  inline const type& ref() const {
    return *__data;
  }
  inline bool valid() const {
    return __data;
  }
  inline void reset() {
    __data = nullptr;
  }
  template <typename V>
  V* getAs() const {
    return static_cast<V*>(__data);
  }
};
template <typename T>
reference<T> make_ref(T &val) {
  return reference<T> { val };
}
template <typename T>
reference<T> make_ref(T *val) {
  return reference<T> { val };
}
}
#endif
