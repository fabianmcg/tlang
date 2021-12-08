#ifndef __AST_ADD_CHILDREN_HH__
#define __AST_ADD_CHILDREN_HH__

#include <static_for.hh>
#include <nodes.hh>
#include <decl_context.hh>

namespace _astnp_ {
namespace __private__ {
template <typename T, typename S>
void addChildren(T *node, S *stack) {
  constexpr NodeClass kind = T::kind;
  if constexpr (kind == NodeClass::ASTNode)
    return;
  else if constexpr (kind == NodeClass::ASTNodeList) {
    for (auto &child : node->template getAs<ASTNodeList>())
      if (child)
        stack->push_back(child.get());
    return;
  } else {
    using children_t = typename T::children_t;
    if constexpr (children_t::size > 0) {
      for (auto &child : **node)
        if (child)
          stack->push_back(child.get());
    }
  }
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
      sf(addChildrenFunction<T, S> { node, stack });
      __private__::addChildren(node, stack);
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
    for (auto &child : **node)
      if (child) {
        stack->push_back(child.get());
      }
  }
};
}
template <typename T, typename S>
void addChildren(T *node, S *stack) {
  using parents_t = typename T::parents_t;
  __private__ ::addChildrenFunction<T, S> { node, stack }.init();
}
}
#endif
