#ifndef AST_VISITORS_ASTVISITOR_HH
#define AST_VISITORS_ASTVISITOR_HH

#include "VisitorHelper.hh"
#include "VisitorOptions.hh"
#include <AST/ASTNode.hh>
#include <deque>
#include <memory>

namespace tlang {
namespace impl {
enum class VisitPattern {
  none,
  pre,
  post,
  prePost
};
inline constexpr VisitPattern toVisitorPattern(VisitorPattern pattern) {
  if (isPreVisit(pattern) && isPostVisit(pattern))
    return VisitPattern::prePost;
  else if (isPreVisit(pattern))
    return VisitPattern::pre;
  else if (isPostVisit(pattern))
    return VisitPattern::post;
  return VisitPattern::none;
}

template <typename Derived, VisitPattern pattern, bool hasPostWalk>
class ASTVisitor;

template <typename Derived, bool hasPostWalk>
class ASTVisitor<Derived, VisitPattern::pre, hasPostWalk> : public ASTVisitorBase {
public:
  using node_t = ASTNode *;
  using stack_t = std::deque<node_t>;
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }

#define AST_MACRO(BASE, PARENT)                                                                                        \
  VisitStatus visit##BASE(BASE *node) {                                                                                \
    return visit;                                                                                                      \
  }
#include <AST/Nodes.inc>

#define ASTNODE(BASE, PARENT)                                                                                          \
  VisitStatus walkUpToASTNode(ASTNode *node) {                                                                         \
    return getDerived().visitASTNode(node);                                                                            \
  }
#define AST_MACRO(BASE, PARENT)                                                                                        \
  VisitStatus walkUpTo##BASE(BASE *node) {                                                                             \
    auto &derived = getDerived();                                                                                      \
    auto walkUpStatus = derived.walkUpTo##PARENT(node->template getAsPtr<PARENT>());                                   \
    if (walkUpStatus != visit)                                                                                         \
      return walkUpStatus;                                                                                             \
    return derived.visit##BASE(node);                                                                                  \
  }
#include <AST/Nodes.inc>

#define AST_MACRO(BASE, PARENT)                                                                                        \
  bool traverse##BASE(BASE *node, stack_t *stack = nullptr) {                                                          \
    auto &derived = getDerived();                                                                                      \
    auto status = visit;                                                                                               \
    status = derived.walkUpTo##BASE(node);                                                                             \
    if (status == terminate)                                                                                           \
      return false;                                                                                                    \
    if constexpr (hasPostWalk) {                                                                                       \
      if (status == visit)                                                                                             \
        status = derived.template postWalk<BASE>(node);                                                                \
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
    return dynamicDispatch(node, nullptr);
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
  bool dynamicDispatch(ASTNode *node, stack_t *stack) {
    if (!node)
      return false;
    auto &derived = getDerived();
    switch (node->classof()) {
#define AST_MACRO(BASE, PARENT)                                                                                        \
  case ASTKind::BASE:                                                                                                  \
    return derived.traverse##BASE(static_cast<BASE *>(node), stack);
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
    void push(ASTNode *node) {
      if (node)
        stack->push_front(node);
    }
    template <typename V>
    static inline StackNodePair<V> make(StackNodePair sn, V *node) {
      return {sn.stack, node};
    }
  };
};

template <typename Derived, bool hasPostWalk>
class ASTVisitor<Derived, VisitPattern::prePost, hasPostWalk> : public ASTVisitorBase {
public:
  typedef enum {
    postVisit,
    preVisit
  } VisitType;
  using node_t = ASTNode *;
  struct VisitInfo {
    node_t node;
    VisitType kind;
  };
  using stack_t = std::deque<VisitInfo>;
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }

#define AST_MACRO(BASE, PARENT)                                                                                        \
  VisitStatus visit##BASE(BASE *node, VisitType kind) {                                                                \
    return visit;                                                                                                      \
  }
#include <AST/Nodes.inc>

#define ASTNODE(BASE, PARENT)                                                                                          \
  VisitStatus walkUpToASTNode(ASTNode *node, VisitType kind) {                                                         \
    return getDerived().visitASTNode(node, kind);                                                                      \
  }
#define AST_MACRO(BASE, PARENT)                                                                                        \
  VisitStatus walkUpTo##BASE(BASE *node, VisitType kind) {                                                             \
    auto &derived = getDerived();                                                                                      \
    auto walkUpStatus = derived.walkUpTo##PARENT(node->template getAsPtr<PARENT>(), kind);                             \
    if (walkUpStatus != visit)                                                                                         \
      return walkUpStatus;                                                                                             \
    return derived.visit##BASE(node, kind);                                                                            \
  }
#include <AST/Nodes.inc>

#define AST_MACRO(BASE, PARENT)                                                                                        \
  bool traverse##BASE(BASE *node, stack_t *stack = nullptr, VisitType kind = preVisit) {                               \
    auto &derived = getDerived();                                                                                      \
    auto status = visit;                                                                                               \
    status = derived.walkUpTo##BASE(node, kind);                                                                       \
    if (status == terminate)                                                                                           \
      return false;                                                                                                    \
    if constexpr (hasPostWalk) {                                                                                       \
      if (status == visit)                                                                                             \
        status = derived.template postWalk<BASE>(node, kind);                                                          \
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
        stack->push_front({node, postVisit});                                                                          \
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
#define AST_MACRO(BASE, PARENT)                                                                                        \
  case ASTKind::BASE:                                                                                                  \
    return derived.traverse##BASE(static_cast<BASE *>(node), stack, info.kind);
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
    void push(ASTNode *node) {
      if (node)
        stack->push_front( { node, preVisit });
    }
    template <typename V>
    static inline StackNodePair<V> make(StackNodePair sn, V *node) {
      return {sn.stack, node};
    }
  };
};

template <typename Derived, bool hasPostWalk>
class ASTVisitor<Derived, VisitPattern::post, hasPostWalk> : public ASTVisitorBase {
public:
  typedef enum {
    postVisit,
    preVisit
  } VisitType;
  using node_t = ASTNode *;
  struct VisitInfo {
    node_t node;
    VisitType kind;
  };
  using stack_t = std::deque<VisitInfo>;
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }

#define AST_MACRO(BASE, PARENT)                                                                                        \
  VisitStatus visit##BASE(BASE *node) {                                                                                \
    return visit;                                                                                                      \
  }
#include <AST/Nodes.inc>

#define ASTNODE(BASE, PARENT)                                                                                          \
  VisitStatus walkUpToASTNode(ASTNode *node) {                                                                         \
    return getDerived().visitASTNode(node);                                                                            \
  }
#define AST_MACRO(BASE, PARENT)                                                                                        \
  VisitStatus walkUpTo##BASE(BASE *node) {                                                                             \
    auto &derived = getDerived();                                                                                      \
    auto walkUpStatus = derived.walkUpTo##PARENT(node->template getAsPtr<PARENT>());                                   \
    if (walkUpStatus != visit)                                                                                         \
      return walkUpStatus;                                                                                             \
    return derived.visit##BASE(node);                                                                                  \
  }
#include <AST/Nodes.inc>

#define AST_MACRO(BASE, PARENT)                                                                                        \
  bool traverse##BASE(BASE *node, stack_t *stack = nullptr, VisitType kind = preVisit) {                               \
    auto &derived = getDerived();                                                                                      \
    auto status = visit;                                                                                               \
    if (kind == postVisit) {                                                                                           \
      status = derived.walkUpTo##BASE(node);                                                                           \
      if (status == terminate)                                                                                         \
        return false;                                                                                                  \
      if constexpr (hasPostWalk) {                                                                                     \
        if (status == visit)                                                                                           \
          status = derived.template postWalk<BASE>(node);                                                              \
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
        stack->push_front({node, postVisit});                                                                          \
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
#define AST_MACRO(BASE, PARENT)                                                                                        \
  case ASTKind::BASE:                                                                                                  \
    return derived.traverse##BASE(static_cast<BASE *>(node), stack, info.kind);
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
    void push(ASTNode *node) {
      if (node)
        stack->push_front( { node, preVisit });
    }
    template <typename V>
    static inline StackNodePair<V> make(StackNodePair sn, V *node) {
      return {sn.stack, node};
    }
  };
};
} // namespace impl

template <typename Derived, VisitorPattern pattern>
using ASTVisitor = impl::ASTVisitor<Derived, impl::toVisitorPattern(pattern), hasPostWalk(pattern)>;

} // namespace tlang
#endif
