#ifndef __DECL_CONTEXT_HH__
#define __DECL_CONTEXT_HH__

#include <list>
#include <memory>
#include <type_traits>
#include "Common/Macros.hh"
#include "Common/Reference.hh"
#include "ASTCommon.hh"

namespace _astnp_ {
struct DeclContext {
  std::list<Decl*>& operator*() {
    return __decls;
  }
  const std::list<Decl*>& operator*() const {
    return __decls;
  }
  void add(Decl *decl) {
    __decls.push_back(decl);
  }
  DeclContext clone() const {
    return DeclContext();
  }
protected:
  std::list<Decl*> __decls;
};
}
#endif
