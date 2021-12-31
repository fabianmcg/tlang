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
  T *data {};
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
