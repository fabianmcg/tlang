#ifndef __SEMA_PARENTS_HH__
#define __SEMA_PARENTS_HH__

#include "AST/Include.hh"
#include <AST/RecursiveASTVisitor.hh>
#include <Common/Utility.hh>
#include <stack>

namespace tlang::sema {
struct ParentSetterAST: RecursiveASTVisitor<ParentSetterAST, VisitorPattern::prePostOrder> {
  ParentSetterAST() {
    stack.push(nullptr);
  }
  visit_t visitASTNode(ASTNode *node, bool isFirst) {
    if (isFirst) {
      node->getParent() = stack.top();
      stack.push(node);
    } else
      stack.pop();
    return visit_value;
  }
  std::stack<ASTNode*> stack { };
};
}
#endif
