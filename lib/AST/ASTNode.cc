#include <AST/ASTNode.hh>
#include <iostream>

namespace tlang {
ASTNode::ASTNode() :
    ASTNode(0) {
}
ASTNode::~ASTNode() = default;

ASTNode::ASTNode(const SourceRange &range) :
    range(range) {
}
ASTNode::ASTNode(const SourceLocation &start, const SourceLocation &end) :
    range(SourceRange { start, end }) {
}
ASTNode::ASTNode(ASTNode &&other) :
    AbstractNode(other.classof()) {
  range = std::exchange(other.range, SourceRange { });
  parent = std::exchange(other.parent, nullptr);
}
ASTNode& ASTNode::operator=(ASTNode &&other) {
  AbstractNode::operator =(std::forward<ASTNode>(*this));
  range = std::exchange(other.range, SourceRange { });
  parent = std::exchange(other.parent, nullptr);
  return *this;
}
ASTNode*& ASTNode::getParent() {
  return parent;
}
ASTNode* ASTNode::getParent() const {
  return parent;
}
SourceLocation ASTNode::getBeginLoc() const {
  return range.begin;
}
SourceLocation ASTNode::getEndLoc() const {
  return range.end;
}
SourceRange& ASTNode::getSourceRange() {
  return range;
}
const SourceRange& ASTNode::getSourceRange() const {
  return range;
}
void ASTNode::dump(std::ostream &ost) const {

}
std::ostream& operator<<(std::ostream &ost, const ASTNode &node) {
  node.dump(ost);
  return ost;
}
}
