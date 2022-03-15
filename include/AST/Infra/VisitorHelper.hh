#ifndef __AST_ADD_CHILDREN_HH__
#define __AST_ADD_CHILDREN_HH__

#include "Common/StaticFor.hh"
#include "AST/ASTNode.hh"
#include "AST/Attr.hh"
#include "AST/Decl.hh"
#include "AST/Type.hh"
#include "AST/Stmt.hh"
#include "AST/Expr.hh"
#include "AST/DeclContext.hh"

namespace _astnp_ {
namespace __private__ {
template <typename T>
struct reverse_wrapper {
  T &container;
  reverse_wrapper(T &container) :
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
};
template <typename T>
reverse_wrapper<T> make_reverse(T &container) {
  return reverse_wrapper<T> { container };
}
template <typename T, typename S>
struct addChildrenVisitor {
  T *node { };
  S *stack { };
  template <int it, typename V>
  void operator()(V &value) {
    using children_t =typename T::children_t;
    constexpr child_kind kind = children_t::children_kinds[it];
    if constexpr (is_list<kind>()) {
      if constexpr (is_dynamic<kind>()) {
        for (auto child : make_reverse(value))
          if (child)
            stack->push_front( { child, true });
      } else {
        for (auto &child : make_reverse(value))
          stack->push_front( { &child, true });
      }
    } else {
      if constexpr (is_dynamic<kind>()) {
        if (value)
          stack->push_front( { value, true });
      } else {
        stack->push_front( { &value, true });
      }
    }
  }
};
template <typename T, typename S>
void addChildren(T *node, S *stack) {
  constexpr NodeClass kind = T::kind;
  if constexpr (kind == NodeClass::ASTNode)
    return;
  auto visitor = addChildrenVisitor<T, S> { node, stack };
  (**node).template traverse<true>(visitor);
}
template <typename T, typename S>
struct addChildrenFunction {
  using parents_t = typename T::parents_t;
  template <int I>
  using type_t = typename parents_t::template type_t<I>;
  T *node;
  S *stack;
  void init() {
    if constexpr (parents_t::size > 0) {
      static_for<0, parents_t::size> sf;
      __private__::addChildren(node, stack);
      sf(addChildrenFunction<T, S> { node, stack });
    }
  }
  template <typename TT>
  inline void walkUp(TT *parent) {
    addChildrenFunction<TT, S> { parent, stack }.init();
  }
  template <int I>
  inline void execute() {
    walkUp(node->template getAsPtr<type_t<I>>());
  }
};
template <typename S>
struct addChildrenFunction<DeclContext, S> {
  DeclContext *node;
  S *stack;
  void init() {
    for (auto &child : make_reverse(*node))
      if (child) {
        stack->push_front(std::pair<ASTNode*, bool>(*child, true));
      }
  }
};
}
template <typename T, typename S>
void addChildren(T *node, S *stack) {
  using parents_t = typename T::parents_t;
  __private__ ::addChildrenFunction<T, S> { node, stack }.init();
}

namespace __private__ {
template <typename T, typename S>
struct visitChildrenVisitor {
  T *node { };
  S &select { };
  template <int it, typename V>
  inline void operator()(V &value) {
    using children_t =typename T::children_t;
    constexpr child_kind kind = children_t::children_kinds[it];
    if constexpr (is_list<kind>()) {
      if constexpr (is_dynamic<kind>()) {
        for (auto child : value)
          if (child)
            select(child);
      } else {
        for (auto &child : value)
          select(&child);
      }
    } else {
      if constexpr (is_dynamic<kind>()) {
        if (value)
          select(value);
      } else {
        select(&value);
      }
    }
  }
};
template <typename T, typename S>
inline void visitChildren(T *node, S &select) {
  constexpr NodeClass kind = T::kind;
  if constexpr (kind == NodeClass::ASTNode)
    return;
  auto visitor = visitChildrenVisitor<T, S> { node, select };
  (**node).template traverse<false>(visitor);
}
template <typename T, typename S>
struct visitChildrenFunction {
  using parents_t = typename T::parents_t;
  template <int I>
  using type_t = typename parents_t::template type_t<I>;
  T *node;
  S &select;
  void init() {
    if constexpr (parents_t::size > 0) {
      static_for<0, parents_t::size> sf;
      sf(visitChildrenFunction<T, S> { node, select });
      __private__::visitChildren(node, select);
    }
  }
  template <typename TT>
  inline void walkUp(TT *parent) {
    visitChildrenFunction<TT, S> { parent, select }.init();
  }
  template <int I>
  inline void execute() {
    walkUp(node->template getAsPtr<type_t<I>>());
  }
};
template <typename S>
struct visitChildrenFunction<DeclContext, S> {
  DeclContext *node;
  S &select;
  void init() {
    for (auto &child : *node)
      if (child)
        select(*child);
  }
};
}
template <typename T, typename S, typename D>
void visitChildren(T *node, S &select, D &derived) {
  using parents_t = typename T::parents_t;
  auto selectMethod = [&](ASTNode *n) {
    select(derived, n);
  };
  __private__ ::visitChildrenFunction<T, decltype(selectMethod)> { node, selectMethod }.init();
}
}
#endif