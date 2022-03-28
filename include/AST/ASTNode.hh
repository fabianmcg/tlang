#ifndef AST_ASTNODE_HH
#define AST_ASTNODE_HH

#include <cstdint>
#include "Common/SourceRange.hh"
#include "Common.hh"
#include "DeclContext.hh"
#include "Infra/ParentList.hh"
#include "Infra/ASTChildren.hh"

namespace tlang {
class AbstractNode {
public:
  AbstractNode() = default;
  AbstractNode(ASTKind kind) :
      kind(kind) {
  }
  ASTKind classof() const {
    return kind;
  }
protected:
  void set(ASTKind kind) {
    this->kind = kind;
  }
private:
  ASTKind kind { };
};

class ASTNode;
template <typename ...T>
using ChildrenList = children_container_helper<ASTNode, T...>;
class ASTNode: public AbstractNode {
public:
  using base_type = AbstractNode;
  using parent_list = ParentClasses<ASTNode>;
  static constexpr ASTKind kind = ASTKind::ASTNode;
  using children_list = ChildrenList<>;
public:
  ASTNode(ASTKind kind) :
      AbstractNode(kind) {
  }
  ASTNode(int) :
      base_type(kind) {
  }
public:
  ASTNode() :
      ASTNode(0) {
  }
//  virtual ~ASTNode() = 0;

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
  ASTNode& operator=(ASTNode &&other) {
    __range = std::exchange(other.__range, SourceRange { });
    __parent = std::exchange(other.__parent, nullptr);
    return *this;
  }
  ASTNode& operator=(const ASTNode&) = default;
  virtual ~ASTNode() = default;
  ASTNode clone() const {
    auto node = ASTNode(__range);
    node.__parent = __parent;
    return node;
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
protected:
  SourceRange __range { };
  ASTNode *__parent { };
};
//inline std::ostream& operator<<(std::ostream &ost, const ASTNode &node) {
//  ost << node.to_string();
//  return ost;
//}
}
#endif
