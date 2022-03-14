#ifndef __AST_ASTNODE_HH__
#define __AST_ASTNODE_HH__

#include <cstdint>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>

#include "Common/Macros.hh"
#include "Common/SourceRange.hh"
#include "ASTCommon.hh"
#include "DeclContext.hh"
#include "Infra/ASTChildren.hh"

namespace _astnp_ {
class ASTNode;
template <typename ...T>
using children_container = children_container_helper<ASTNode, T...>;
class ASTNode {
public:
  using parents_t = parent_container<>;
  using node_kind_t = NodeClass;
  static constexpr node_kind_t kind = node_kind_t::ASTNode;
  virtual node_kind_t classOf() const {
    return kind;
  }
  bool is(node_kind_t k) const {
    return classOf() == k;
  }
  bool isNot(node_kind_t k) const {
    return classOf() != k;
  }
  virtual bool isClass(node_kind_t k) const {
    return kind == k;
  }
  using children_t = children_container<>;
  ASTNode() = default;
  ASTNode(const SourceRange &range) :
      __range(range) {
  }
  ASTNode(const SourceLocation &start, const SourceLocation &end = SourceLocation { }) :
      __range(SourceRange { start, end }) {
  }
  ASTNode(ASTNode &&other) {
    __range = std::exchange(other.__range, SourceRange { });
    __parent = std::exchange(other.__parent, nullptr);
  }
  ASTNode(const ASTNode&) = default;
  ASTNode& operator =(ASTNode &&other) {
    __range = std::exchange(other.__range, SourceRange { });
    __parent = std::exchange(other.__parent, nullptr);
    return *this;
  }
  ASTNode& operator =(const ASTNode&) = default;
  virtual ~ASTNode() = default;
  ASTNode clone() const {
    auto node = ASTNode(__range);
    node.__parent = __parent;
    return node;
  }
  children_t* operator->() {
    return &__children;
  }
  const children_t* operator->() const {
    return &__children;
  }
  children_t& operator*() {
    return __children;
  }
  const children_t& operator*() const {
    return __children;
  }
  ASTNode*& parent() {
    return __parent;
  }
  ASTNode* parent() const {
    return __parent;
  }
  auto getBeginLoc() const {
    return __range.begin;
  }
  auto getEndLoc() const {
    return __range.end;
  }
  SourceRange& getSourceRange() {
    return __range;
  }
  const SourceRange& getSourceRange() const {
    return __range;
  }
  template <typename T>
  T* getAsPtr() {
    return dynamic_cast<T*>(this);
  }
  template <typename T>
  const T* getAsPtr() const {
    return dynamic_cast<const T*>(this);
  }
  template <typename T>
  T& getAs() {
    return *static_cast<T*>(this);
  }
  template <typename T>
  const T& getAs() const {
    return *static_cast<const T*>(this);
  }
  inline bool isAttr() const {
    return _astnp_::isAttr(classOf());
  }
  inline bool isType() const {
    return _astnp_::isType(classOf());
  }
  inline bool isDecl() const {
    return _astnp_::isDecl(classOf());
  }
  inline bool isStmt() const {
    return _astnp_::isStmt(classOf());
  }
  virtual std::string to_string() const {
    return _astnp_::to_string(classOf()) + __range.to_string();
  }
protected:
  SourceRange __range { };
  ASTNode *__parent { };
  children_container<> __children;
};
inline std::ostream& operator<<(std::ostream &ost, const ASTNode &node) {
  ost << node.to_string();
  return ost;
}
}
#endif
