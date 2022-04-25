#ifndef AST_ANYNODE_HH
#define AST_ANYNODE_HH

#include <AST/Common.hh>
#include <AST/Traits.hh>
#include <cassert>

namespace tlang {
struct AnyASTNodeRef {
public:
  AnyASTNodeRef() = default;
  AnyASTNodeRef(AnyASTNodeRef&&) = default;
  AnyASTNodeRef(const AnyASTNodeRef&) = default;
  AnyASTNodeRef& operator=(AnyASTNodeRef&&) = default;
  AnyASTNodeRef& operator=(const AnyASTNodeRef&) = default;
  template <typename T, std::enable_if_t<!std::is_same_v<T, AnyASTNodeRef>, int> = 0>
  explicit AnyASTNodeRef(T &node) {
    static_assert(isValidNode_v<T>);
    this->node = &node;
    kind = node.classof();
  }
  template <typename T>
  AnyASTNodeRef(T *&node) {
    static_assert(isValidNode_v<T>);
    this->node = &node;
    kind = T::kind;
    fromPtr = true;
  }
  ~AnyASTNodeRef() {
    node = nullptr;
    kind = { };
    fromPtr = false;
  }
  operator bool() const {
    return node;
  }
  template <typename T>
  T* get() {
    if (fromPtr) {
      if (kind == T::kind)
        return *reinterpret_cast<T**>(node);
    } else {
      if (ASTTraits_t<T>::is(kind))
        return reinterpret_cast<T*>(node);
    }
    return nullptr;
  }
  template <typename S>
  bool makeNull() {
    assert(this->node);
    if (S::kind == kind) {
      if (fromPtr)
        *reinterpret_cast<S**>(this->node) = nullptr;
      else
        return false;
      return true;
    }
    return false;
  }
  template <typename S, typename T>
  bool assign(T *node) {
    assert(this->node);
    if (S::kind == kind) {
      if (fromPtr)
        *reinterpret_cast<S**>(this->node) = node;
      else
        *reinterpret_cast<S*>(this->node) = *node;
      return true;
    }
    return false;
  }
  template <typename T>
  bool dynAssign(T *node) {
    assert(this->node);
    switch (kind) {
#define AST_MACRO(BASE, PARENT)                                                                                        \
  case ASTKind::BASE:                                                                                                  \
    if (fromPtr) {                                                                                                 \
      if (isa<BASE>(node)) {                                                                                           \
        *reinterpret_cast<BASE **>(this->node) = node;                                                                 \
        return true;                                                                                                   \
      }                                                                                                                \
      return false;                                                                                                    \
    } else {                                                                                                           \
      if (ASTKind::BASE == node->classof()) {                                                                          \
        *reinterpret_cast<BASE *>(this->node) = *static_cast<BASE *>(node);                                            \
        return true;                                                                                                   \
      }                                                                                                                \
      return false;                                                                                                    \
    }
#include <AST/Nodes.inc>
    default:
      return false;
    }
  }
  ASTKind classof() const {
    return kind;
  }
  bool isFromPtr() const {
    return fromPtr;
  }

private:
  void *node { };
  ASTKind kind { };
  bool fromPtr { };
};
template <typename T>
struct ASTData {
  using type = T;
  ASTData(T *node, AnyASTNodeRef reference = { }) :
      node(node), reference(reference) {
  }
  T* operator*() {
    return node;
  }
  AnyASTNodeRef* operator->() {
    return &reference;
  }
  template <typename V>
  V* get() {
    return dyn_cast<V>(node);
  }
  T *node;
  AnyASTNodeRef reference;
};
} // namespace tlang

#endif
