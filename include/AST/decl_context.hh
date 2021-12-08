#ifndef __DECL_CONTEXT_HH__
#define __DECL_CONTEXT_HH__

#include <list>
#include <memory>
#include <type_traits>
#include "macros.hh"
#include "nodes.hh"

namespace _astnp_ {
using Identifier = std::string;
struct DeclContext {
  std::list<std::unique_ptr<Decl>>& operator*() {
    return decls;
  }
  const std::list<std::unique_ptr<Decl>>& operator*() const {
    return decls;
  }
  void addDecl(std::unique_ptr<Decl> &&decl) {
    decls.push_back(std::forward<std::unique_ptr<Decl>>(decl));
  }
  std::list<std::unique_ptr<Decl>> decls;
};
}
#endif
