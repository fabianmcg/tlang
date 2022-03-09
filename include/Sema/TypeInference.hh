#ifndef __SEMA_TYPEINFERENCE_HH__
#define __SEMA_TYPEINFERENCE_HH__

#include "AST/Include.hh"
#include <AST/RecursiveASTVisitor.hh>
#include <Common/Utility.hh>
#include <deque>
#include <map>
#include <string>

namespace tlang::sema {
struct TypeInferenceAST: RecursiveASTVisitor<TypeInferenceAST, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>, false, true> {
  TypeInferenceAST(ASTContext &context) :
      context(context) {
  }
  visit_t visitCallExpr(CallExpr *node, bool isFirst) {
    if (isFirst)
      if (auto ref = dynamic_cast<DeclRefExpr*>(node->getCallee()))
        node->getType() = ref->getType();
    return visit_value;
  }
  visit_t visitArrayExpr(ArrayExpr *node, bool isFirst) {
    if (!isFirst) {
      auto qtype = node->getArray()->getType();
      if (auto type = dynamic_cast<ArrayType*>(qtype.getType())) {
        if (type->getLayout().size() != node->getIndex().size())
          throw(std::runtime_error("Invalid index for array"));
        Type *t = type->getUnderlying();
        node->getType() = QualType((QualType::cvr_qualifiers_t) qtype.getQualifiers(), std::move(t));
      } else if (auto type = dynamic_cast<PtrType*>(qtype.getType())) {
        if (node->getIndex().size() != 1)
          throw(std::runtime_error("Invalid index for array"));
        Type *t = type->getUnderlying();
        node->getType() = QualType((QualType::cvr_qualifiers_t) qtype.getQualifiers(), std::move(t));
      }
    }
    return visit_value;
  }
  visit_t visitUnaryOperation(UnaryOperation *node, bool isFirst) {
    if (!isFirst) {
      auto op = node->getOperator();
      if (op == UnaryOperation::Dereference) {
        node->getType() = QualType(QualType::None, context.add_type(PtrType(Type(), node->getExpr()->getType().getType())));
      }
    }
    return visit_value;
  }
  visit_t visitBinaryOperation(BinaryOperation *node, bool isFirst) {
    if (!isFirst) {
      auto op = node->getOperator();
      if (op == OperatorKind::Assign) {
        node->getType() = node->getLhs()->getType();
      }
    }
    return visit_value;
  }
  ASTContext &context;
};
inline void TypeInferenceSemaPass(ASTContext &ctx) {
  TypeInferenceAST { ctx }.traverseModuleDecl(*ctx);
}
}
#endif
