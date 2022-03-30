#ifndef AST_ASTNODE_HH
#define AST_ASTNODE_HH

#include <iosfwd>
#include <Basic/SourceRange.hh>
#include <Basic/ParentList.hh>
#include <Basic/ChildrenList.hh>
#include <ADT/Identifier.hh>
#include <ADT/Reference.hh>
#include <AST/Common.hh>
#include <AST/Traits.hh>

namespace tlang {
class AbstractNode {
protected:
  AbstractNode(ASTKind kind) :
      kind(kind) {
  }
public:
  AbstractNode() = default;
  ASTKind classof() const {
    return kind;
  }
  AbstractNode(const AbstractNode &other) :
      kind(other.kind) {
  }
  AbstractNode(AbstractNode &&other) :
      kind(other.kind) {
  }
  AbstractNode& operator=(const AbstractNode &other) {
    kind = other.kind;
    return *this;
  }
  AbstractNode& operator=(AbstractNode &&other) {
    kind = other.kind;
    return *this;
  }
protected:
  void set(ASTKind kind) {
    this->kind = kind;
  }
private:
  ASTKind kind { };
};

class ASTNode: public AbstractNode {
public:
  using base_type = AbstractNode;
  using parent_list = ParentList<>;
  using children_list = ChildrenList<>;
  static constexpr ASTKind kind = ASTKind::ASTNode;
protected:
  ASTNode(int) :
      base_type(kind) {
  }
public:
  ASTNode();
  virtual ~ASTNode() = 0;
  ASTNode(const SourceRange &range);
  ASTNode(const SourceLocation &start, const SourceLocation &end = SourceLocation { });
  ASTNode(ASTNode &&other);
  ASTNode(const ASTNode&) = default;
  ASTNode& operator=(ASTNode &&other);
  ASTNode& operator=(const ASTNode&) = default;
  ASTNode*& getParent();
  ASTNode* getParent() const;
  SourceLocation getBeginLoc() const;
  SourceLocation getEndLoc() const;
  SourceRange& getSourceRange();
  const SourceRange& getSourceRange() const;
  template <typename T>
  T& getAs() {
    return *dyn_cast<T>(this);
  }
  template <typename T>
  const T& getAs() const {
    return *dyn_cast<const T>(this);
  }
  template <typename T>
  T* getAsPtr() {
    return dyn_cast<T>(this);
  }
  template <typename T>
  const T* getAsPtr() const {
    return dyn_cast<const T>(this);
  }
  void dump(std::ostream &ost) const;
  static constexpr bool hasChildren() {
    return children_list::size;
  }
protected:
  SourceRange range { };
  ASTNode *parent { };
};
std::ostream& operator<<(std::ostream &ost, const ASTNode &node);
}
#endif
