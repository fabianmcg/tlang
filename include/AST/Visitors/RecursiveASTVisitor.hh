#ifndef AST_RECURSIVEASTVISITOR_HH
#define AST_RECURSIVEASTVISITOR_HH

#include <deque>
#include <memory>
#include <AST/VisitorHelper.hh>
#include <AST/ASTNode.hh>

namespace tlang {
enum class VisitStatus {
  visit,
  skip,
  terminate
};
template <typename T>
struct VisitReturn;
template <>
struct VisitReturn<bool> {
  using type = bool;
  static constexpr type visit_value = true;
  static constexpr type terminate_value = false;
};
template <>
struct VisitReturn<VisitStatus> {
  using type = VisitStatus;
  static constexpr type visit_value = VisitStatus::visit;
  static constexpr type terminate_value = VisitStatus::terminate;
};
enum class VisitorPattern {
  preOrder = 1,
  postOrder = 2,
  prePostOrder = 3,
};
inline constexpr VisitorPattern operator|(VisitorPattern x, VisitorPattern y) {
  return static_cast<VisitorPattern>(static_cast<int>(x) | static_cast<int>(y));
}
inline constexpr VisitorPattern operator&(VisitorPattern x, VisitorPattern y) {
  return static_cast<VisitorPattern>(static_cast<int>(x) & static_cast<int>(y));
}
inline constexpr bool isPreVisit(VisitorPattern x) {
  return (x & VisitorPattern::preOrder) == VisitorPattern::preOrder;
}
inline constexpr bool isPostVisit(VisitorPattern x) {
  return (x & VisitorPattern::postOrder) == VisitorPattern::postOrder;
}

#define MAKE_STACK                                                                                                     \
  std::unique_ptr<stack_t> _stack;                                                                              \
  if (!stack) {                                                                                                        \
    _stack = std::make_unique<stack_t>();                                                                              \
    stack = _stack.get();                                                                                              \
  }
#define WALKUP_MACRO(PARENT, CLASS)                                                                                    \
  auto &derived = getDerived();                                                                                        \
  auto walkUpStatus = derived.walkUpTo##PARENT(node->template getAsPtr<PARENT>(), firstQ);                             \
  if (walkUpStatus != visit_value)                                                                                     \
    return walkUpStatus;                                                                                               \
  return derived.visit##CLASS(node, firstQ);
#define TRAVERSE_MACRO(CLASS)                                                                                          \
  auto &derived = getDerived();                                                                                        \
  auto status = visit_value;                                                                                           \
  if (isPreVisit(pattern) || !firstQ) {                                                                                \
    status = derived.walkUpTo##CLASS(node, firstQ);                                                                    \
    if (status == terminate_value)                                                                                     \
      return false;                                                                                                    \
  }                                                                                                                    \
  if constexpr (after_walk) {                                                                                          \
    status = derived.template postWalk<CLASS>(node, firstQ);                                                           \
    if (status == terminate_value)                                                                                     \
      return false;                                                                                                    \
  }                                                                                                                    \
  MAKE_STACK                                                                                                           \
  if (isPostVisit(pattern) && firstQ)                                                                                  \
    stack->push_front(node_t{node, false});                                                                            \
  if (firstQ && status == visit_value)                                                                                 \
    addChildren<CLASS>(node, stack);                                                                                   \
  if (_stack && !traverseStack(stack))                                                                                 \
    return false;                                                                                                      \
  return true;
template <typename Derived, VisitorPattern pattern = VisitorPattern::preOrder, typename VisitReturnType = VisitReturn<bool>,
    bool after_walk = false, bool modifiable = false>
class RecursiveASTVisitor {
public:
  using node_t = std::pair<ASTNode *, bool>;
  using stack_t = std::deque<node_t>;
  using visit_return_type = VisitReturnType;
  using visit_t = typename VisitReturnType::type;
  static constexpr visit_t visit_value = visit_return_type::visit_value;
  static constexpr visit_t terminate_value = visit_return_type::terminate_value;
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }

  bool dynamicTraverse(ASTNode *node) {
    return select(getDerived(), node, nullptr, true);
  }

protected:
  bool traverseStack(stack_t *stack) {
    while (!stack->empty()) {
      auto top = stack->front();
      stack->pop_front();
      if (!top.first)
        continue;
      if (!select(getDerived(), top.first, stack, top.second))
        return false;
    }
    return true;
  }

};
#undef WALKUP_MACRO
#undef TRAVERSE_MACRO
#undef MAKE_STACK

#define WALKUP_MACRO(PARENT, CLASS)                                                                                    \
  auto &derived = getDerived();                                                                                        \
  auto walkUpStatus = derived.walkUpTo##PARENT(node->template getAsPtr<PARENT>(), firstQ);                             \
  if (walkUpStatus != visit_value)                                                                                     \
    return walkUpStatus;                                                                                               \
  return derived.visit##CLASS(node, firstQ);
#define TRAVERSE_MACRO(CLASS)                                                                                          \
  auto &derived = getDerived();                                                                                        \
  auto status = visit_value;                                                                                           \
  if constexpr (isPreVisit(pattern)) {                                                                                 \
    status = derived.walkUpTo##CLASS(node, true);                                                                      \
    if (status == terminate_value)                                                                                     \
      return false;                                                                                                    \
  }                                                                                                                    \
  if constexpr (after_walk) {                                                                                          \
    status = derived.template postWalk<CLASS>(node);                                                                   \
    if (status == terminate_value)                                                                                     \
      return false;                                                                                                    \
  }                                                                                                                    \
  if (status == visit_value)                                                                                           \
    visitChildren<CLASS>(node, select, getDerived());                                                                  \
  if constexpr (isPostVisit(pattern)) {                                                                                \
    status = derived.walkUpTo##CLASS(node, false);                                                                     \
    if (status == terminate_value)                                                                                     \
      return false;                                                                                                    \
  }                                                                                                                    \
  return true;

} // namespace _astnp_
#endif
