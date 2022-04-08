#ifndef AST_VISITORS_EDITVISITOR_HH
#define AST_VISITORS_EDITVISITOR_HH

#include "EditHelper.hh"
#include "VisitorOptions.hh"
#include <AST/ASTNode.hh>
#include <deque>
#include <memory>

namespace tlang {
namespace impl {
template <typename Derived, VisitPattern pattern, bool hasPostWalk>
class EditVisitor;

template <typename Derived, bool hasPostWalk>
class EditVisitor<Derived, VisitPattern::pre, hasPostWalk> : public ASTVisitorBase {
public:
  using node_t = std::pair<ASTNode *, AnyASTNodeRef>;
  using stack_t = std::deque<node_t>;
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }

#define AST_MACRO(BASE, PARENT)                                                                                        \
  VisitStatus visit##BASE(BASE *node, AnyASTNodeRef &ref) {                                                            \
    return visit;                                                                                                      \
  }
#include <AST/Nodes.inc>

#define ASTNODE(BASE, PARENT)                                                                                          \
  VisitStatus walkUpToASTNode(ASTNode *node, AnyASTNodeRef &ref) {                                                     \
    return getDerived().visitASTNode(node, ref);                                                                       \
  }
#define AST_MACRO(BASE, PARENT)                                                                                        \
  VisitStatus walkUpTo##BASE(BASE *node, AnyASTNodeRef &ref) {                                                         \
    auto &derived = getDerived();                                                                                      \
    auto walkUpStatus = derived.walkUpTo##PARENT(node->template getAsPtr<PARENT>(), ref);                              \
    if (walkUpStatus != visit)                                                                                         \
      return walkUpStatus;                                                                                             \
    return derived.visit##BASE(node, ref);                                                                             \
  }
#include <AST/Nodes.inc>

#define NO_ABSTRACT
#define AST_MACRO(BASE, PARENT)                                                                                        \
  bool traverse##BASE(BASE *node, AnyASTNodeRef ref, stack_t *stack = nullptr) {                                       \
    auto &derived = getDerived();                                                                                      \
    auto status = visit;                                                                                               \
    status = derived.walkUpTo##BASE(node, ref);                                                                        \
    if (status == terminate)                                                                                           \
      return false;                                                                                                    \
    if constexpr (hasPostWalk) {                                                                                       \
      if (status == visit)                                                                                             \
        status = derived.template postWalk<BASE>(node, ref);                                                           \
      if (status == terminate)                                                                                         \
        return false;                                                                                                  \
    }                                                                                                                  \
    std::unique_ptr<stack_t> _stack;                                                                                   \
    if (!stack) {                                                                                                      \
      _stack = std::make_unique<stack_t>();                                                                            \
      stack = _stack.get();                                                                                            \
    }                                                                                                                  \
    if (status == visit)                                                                                               \
      impl::AddChildren{}(StackNodePair<BASE>{stack, node});                                                           \
    if (_stack && !traverseStack(stack))                                                                               \
      return false;                                                                                                    \
    return true;                                                                                                       \
  }
#include <AST/Nodes.inc>

  bool dynamicTraverse(ASTNode *node) {
    return dynamicDispatch( { node, AnyASTNodeRef { } }, nullptr);
  }

private:
  bool traverseStack(stack_t *stack) {
    while (!stack->empty()) {
      auto top = stack->front();
      stack->pop_front();
      if (!dynamicDispatch(top, stack))
        return false;
    }
    return true;
  }
  bool dynamicDispatch(node_t node_data, stack_t *stack) {
    auto node = node_data.first;
    if (!node)
      return false;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define AST_MACRO(BASE, PARENT)                                                                                        \
  case ASTKind::BASE:                                                                                                  \
    return derived.traverse##BASE(static_cast<BASE *>(node), node_data.second, stack);
#include <AST/Nodes.inc>
    default:
      return true;
    }
  }
  template <typename Node>
  struct StackNodePair {
    using node_type = Node;
    stack_t *stack;
    Node *node;
    Node* operator*() {
      return node;
    }
    void push(ASTNode *node, AnyASTNodeRef ref) {
      if (node)
        stack->push_front( { node, ref });
    }
    template <typename V>
    static inline StackNodePair<V> make(StackNodePair sn, V *node) {
      return {sn.stack, node};
    }
  };
};

template <typename Derived, bool hasPostWalk>
class EditVisitor<Derived, VisitPattern::prePost, hasPostWalk> : public ASTVisitorBase {
public:
  typedef enum {
    postVisit,
    preVisit
  } VisitType;
  using node_t = ASTNode *;
  struct VisitInfo {
    node_t node;
    VisitType kind;
    AnyASTNodeRef ref;
  };
  using stack_t = std::deque<VisitInfo>;
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }

#define AST_MACRO(BASE, PARENT)                                                                                        \
  VisitStatus visit##BASE(BASE *node, AnyASTNodeRef &ref, VisitType kind) {                                            \
    return visit;                                                                                                      \
  }
#include <AST/Nodes.inc>

#define ASTNODE(BASE, PARENT)                                                                                          \
  VisitStatus walkUpToASTNode(ASTNode *node, AnyASTNodeRef &ref, VisitType kind) {                                     \
    return getDerived().visitASTNode(node, ref, kind);                                                                 \
  }
#define AST_MACRO(BASE, PARENT)                                                                                        \
  VisitStatus walkUpTo##BASE(BASE *node, AnyASTNodeRef &ref, VisitType kind) {                                         \
    auto &derived = getDerived();                                                                                      \
    auto walkUpStatus = derived.walkUpTo##PARENT(node->template getAsPtr<PARENT>(), ref, kind);                        \
    if (walkUpStatus != visit)                                                                                         \
      return walkUpStatus;                                                                                             \
    return derived.visit##BASE(node, ref, kind);                                                                       \
  }
#include <AST/Nodes.inc>

#define NO_ABSTRACT
#define AST_MACRO(BASE, PARENT)                                                                                        \
  bool traverse##BASE(BASE *node, AnyASTNodeRef ref, stack_t *stack = nullptr, VisitType kind = preVisit) {            \
    auto &derived = getDerived();                                                                                      \
    auto status = visit;                                                                                               \
    status = derived.walkUpTo##BASE(node, ref, kind);                                                                  \
    if (status == terminate)                                                                                           \
      return false;                                                                                                    \
    if constexpr (hasPostWalk) {                                                                                       \
      if (status == visit)                                                                                             \
        status = derived.template postWalk<BASE>(node, ref, kind);                                                     \
      if (status == terminate)                                                                                         \
        return false;                                                                                                  \
    }                                                                                                                  \
    if (kind == preVisit) {                                                                                            \
      std::unique_ptr<stack_t> _stack;                                                                                 \
      if (!stack) {                                                                                                    \
        _stack = std::make_unique<stack_t>();                                                                          \
        stack = _stack.get();                                                                                          \
      }                                                                                                                \
      if (status == visit)                                                                                             \
        stack->push_front({node, ref, postVisit});                                                                     \
      if (status == visit)                                                                                             \
        impl::AddChildren{}(StackNodePair<BASE>{stack, node});                                                         \
      if (_stack && !traverseStack(stack))                                                                             \
        return false;                                                                                                  \
    }                                                                                                                  \
    return true;                                                                                                       \
  }
#include <AST/Nodes.inc>

  bool dynamicTraverse(ASTNode *node) {
    return dynamicDispatch(VisitInfo { node, preVisit }, nullptr);
  }

private:
  bool traverseStack(stack_t *stack) {
    while (!stack->empty()) {
      auto top = stack->front();
      stack->pop_front();
      if (!dynamicDispatch(top, stack))
        return false;
    }
    return true;
  }
  bool dynamicDispatch(VisitInfo info, stack_t *stack) {
    auto node = info.node;
    if (!node)
      return false;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define AST_MACRO(BASE, PARENT)                                                                                        \
  case ASTKind::BASE:                                                                                                  \
    return derived.traverse##BASE(static_cast<BASE *>(node), info.ref, stack, info.kind);
#include <AST/Nodes.inc>
    default:
      return true;
    }
  }
  template <typename Node>
  struct StackNodePair {
    using node_type = Node;
    stack_t *stack;
    Node *node;
    Node* operator*() {
      return node;
    }
    void push(ASTNode *node, AnyASTNodeRef ref) {
      if (node)
        stack->push_front( { node, preVisit, ref });
    }
    template <typename V>
    static inline StackNodePair<V> make(StackNodePair sn, V *node) {
      return {sn.stack, node};
    }
  };
};

template <typename Derived, bool hasPostWalk>
class EditVisitor<Derived, VisitPattern::post, hasPostWalk> : public ASTVisitorBase {
public:
  typedef enum {
    postVisit,
    preVisit
  } VisitType;
  using node_t = ASTNode *;
  struct VisitInfo {
    node_t node;
    VisitType kind;
    AnyASTNodeRef ref;
  };
  using stack_t = std::deque<VisitInfo>;
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }

#define AST_MACRO(BASE, PARENT)                                                                                        \
  VisitStatus visit##BASE(BASE *node, AnyASTNodeRef &ref) {                                                            \
    return visit;                                                                                                      \
  }
#include <AST/Nodes.inc>

#define ASTNODE(BASE, PARENT)                                                                                          \
  VisitStatus walkUpToASTNode(ASTNode *node, AnyASTNodeRef &ref) {                                                     \
    return getDerived().visitASTNode(node);                                                                            \
  }
#define AST_MACRO(BASE, PARENT)                                                                                        \
  VisitStatus walkUpTo##BASE(BASE *node, AnyASTNodeRef &ref) {                                                         \
    auto &derived = getDerived();                                                                                      \
    auto walkUpStatus = derived.walkUpTo##PARENT(node->template getAsPtr<PARENT>(), ref);                              \
    if (walkUpStatus != visit)                                                                                         \
      return walkUpStatus;                                                                                             \
    return derived.visit##BASE(node, ref);                                                                             \
  }
#include <AST/Nodes.inc>

#define NO_ABSTRACT
#define AST_MACRO(BASE, PARENT)                                                                                        \
  bool traverse##BASE(BASE *node, AnyASTNodeRef ref, stack_t *stack = nullptr, VisitType kind = preVisit) {            \
    auto &derived = getDerived();                                                                                      \
    auto status = visit;                                                                                               \
    if (kind == postVisit) {                                                                                           \
      status = derived.walkUpTo##BASE(node, ref);                                                                      \
      if (status == terminate)                                                                                         \
        return false;                                                                                                  \
      if constexpr (hasPostWalk) {                                                                                     \
        if (status == visit)                                                                                           \
          status = derived.template postWalk<BASE>(node, ref);                                                         \
        if (status == terminate)                                                                                       \
          return false;                                                                                                \
      }                                                                                                                \
    }                                                                                                                  \
    if (kind == preVisit) {                                                                                            \
      std::unique_ptr<stack_t> _stack;                                                                                 \
      if (!stack) {                                                                                                    \
        _stack = std::make_unique<stack_t>();                                                                          \
        stack = _stack.get();                                                                                          \
      }                                                                                                                \
      if (status == visit)                                                                                             \
        stack->push_front({node, postVisit, ref});                                                                     \
      if (status == visit)                                                                                             \
        impl::AddChildren{}(StackNodePair<BASE>{stack, node});                                                         \
      if (_stack && !traverseStack(stack))                                                                             \
        return false;                                                                                                  \
    }                                                                                                                  \
    return true;                                                                                                       \
  }
#include <AST/Nodes.inc>

  bool dynamicTraverse(ASTNode *node) {
    return dynamicDispatch(VisitInfo { node, preVisit }, nullptr);
  }

private:
  bool traverseStack(stack_t *stack) {
    while (!stack->empty()) {
      auto top = stack->front();
      stack->pop_front();
      if (!dynamicDispatch(top, stack))
        return false;
    }
    return true;
  }
  bool dynamicDispatch(VisitInfo info, stack_t *stack) {
    auto node = info.node;
    if (!node)
      return false;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define AST_MACRO(BASE, PARENT)                                                                                        \
  case ASTKind::BASE:                                                                                                  \
    return derived.traverse##BASE(static_cast<BASE *>(node), info.ref, stack, info.kind);
#include <AST/Nodes.inc>
    default:
      return true;
    }
  }
  template <typename Node>
  struct StackNodePair {
    using node_type = Node;
    stack_t *stack;
    Node *node;
    Node* operator*() {
      return node;
    }
    void push(ASTNode *node, AnyASTNodeRef ref) {
      if (node)
        stack->push_front( { node, preVisit, ref });
    }
    template <typename V>
    static inline StackNodePair<V> make(StackNodePair sn, V *node) {
      return {sn.stack, node};
    }
  };
};
} // namespace impl

template <typename Derived, VisitorPattern pattern>
using EditVisitor = impl::EditVisitor<Derived, impl::toVisitorPattern(pattern), hasPostWalk(pattern)>;

} // namespace tlang
#endif
