#ifndef __DECL_CONTEXT_HH__
#define __DECL_CONTEXT_HH__

#include <list>
#include <memory>
#include <type_traits>
#include "Common/Macros.hh"
#include "ASTCommon.hh"

namespace _astnp_ {
template <typename T>
struct reference {
  T *__data { };
  reference() = default;
  ~reference() {
    __data = nullptr;
  }
  reference(reference&&) = default;
  reference(const reference&) = default;
  reference& operator=(reference&&) = default;
  reference& operator=(const reference&) = default;
  reference& operator=(T &value) {
    __data = &value;
    return *this;
  }
  reference& operator=(T *value) {
    __data = value;
    return *this;
  }
  operator bool() const {
    return __data;
  }
  T* operator*() {
    return __data;
  }
  const T* operator*() const {
    return __data;
  }
  T* data() {
    return __data;
  }
  const T* data() const {
    return __data;
  }
  T& ref() {
    return *__data;
  }
  const T& ref() const {
    return *__data;
  }
  inline bool valid() const {
    return __data;
  }
  void reset() {
    __data = nullptr;
  }
};
struct DeclContext {
  std::list<std::unique_ptr<Decl>>& operator*() {
    return decls;
  }
  const std::list<std::unique_ptr<Decl>>& operator*() const {
    return decls;
  }
  void add(std::unique_ptr<Decl> &&decl) {
    decls.push_back(std::forward<std::unique_ptr<Decl>>(decl));
  }
  DeclContext clone() const {
    return DeclContext();
  }
  std::list<std::unique_ptr<Decl>> decls;
};
}
#endif
