#include <deque>
#include <map>
#include <string>
#include <Sema/Sema.hh>
#include <AST/Visitors/ASTVisitor.hh>

namespace tlang {
namespace sema {
struct TypeInferenceAST: ASTVisitor<TypeInferenceAST, VisitorPattern::prePostOrder> {
  TypeInferenceAST(ASTContext &context) :
      context(context) {
  }
  visit_t visitDeclRefExpr(DeclRefExpr *node, VisitType isFirst) {
    if (isFirst) {
      if (auto symbol = declContext->find(node->getIdentifier(), false)) {
        auto decl = *symbol;
        if (auto vd = dyn_cast<VariableDecl>(decl)) {
          node->getType() = vd->getType().addQuals(QualType::Reference);
        } else if (auto fd = dyn_cast<FunctionDecl>(decl)) {
          node->getType() = QualType(fd->getType(context.types()));
        }
      } else
        throw(std::runtime_error("Undefined symbol: " + node->getIdentifier()));
    }
    return visit;
  }
  visit_t visitCallExpr(CallExpr *node, VisitType isFirst) {
    if (isFirst == postVisit)
      if (auto ref = dyn_cast<DeclRefExpr>(node->getCallee()))
        if (auto ft = dyn_cast<FunctionType>(ref->getType().getType()))
          node->getType() = ft->getReturnType();
    return visit;
  }
  visit_t visitArrayExpr(ArrayExpr *node, VisitType isFirst) {
    if (isFirst == postVisit) {
      auto qualType = node->getArray()->getType();
      if (auto type = dyn_cast<ArrayType>(qualType.getType())) {
        if (type->getLayout().size() != node->getIndex().size())
          throw(std::runtime_error("Invalid index for array"));
        node->getType() = QualType((QualType::cvr_qualifiers) (QualType::Reference | qualType.getQualifiers()), type->getUnderlying());
      } else if (auto type = dynamic_cast<PtrType*>(qualType.getType())) {
        if (node->getIndex().size() != 1)
          throw(std::runtime_error("Invalid index for array"));
        node->getType() = QualType((QualType::cvr_qualifiers) (QualType::Reference | qualType.getQualifiers()), type->getUnderlying());
      } else {
        throw(std::runtime_error("Invalid ArrayExpr"));
      }
    }
    return visit;
  }
  visit_t visitUnaryOperator(UnaryOperator *node, VisitType isFirst) {
    if (isFirst == postVisit) {
      auto op = node->getOp();
      auto &exprType = node->getExpr()->getType();
      if (op == UnaryOperator::Dereference) {
        if (auto pt = dyn_cast<PtrType>(exprType.getType()))
          node->getType() = QualType(QualType::Reference, pt->getUnderlying());
        else
          throw(std::runtime_error("Invalid dereference"));
      } else if (op == UnaryOperator::Address)
        node->getType() = QualType(PtrType::get(&context.types(), exprType.getType()));
      else {
        if (isArithmetic(exprType.getType()))
          node->getType() = exprType;
        else
          throw(std::runtime_error("Invalid unary operator for non arithmetic type"));
      }
    }
    return visit;
  }
  visit_t visitBinaryOperator(BinaryOperator *node, VisitType isFirst) {
    if (isFirst == postVisit) {
      auto op = node->getOp();
      Expr *&lhs = node->getLhs(), *&rhs = node->getRhs();
      if (op == BinaryOperator::Assign) {
        node->getType() = lhs->getType().addQuals(QualType::Reference);
        if (lhs->getType().getType() != rhs->getType().getType())
          rhs = context.make<ImplicitCastExpr>(rhs, lhs->getType().modQuals());
      } else {
        auto trr = typePromotion(lhs->getType().getType()->getCanonicalType(), rhs->getType().getType()->getCanonicalType());
        if (!trr.first)
          throw(std::runtime_error("Invalid BinOp"));
        if (trr.second == 0)
          lhs = context.make<ImplicitCastExpr>(lhs, rhs->getType().modQuals());
        else if (trr.second == 1)
          rhs = context.make<ImplicitCastExpr>(rhs, lhs->getType().modQuals());
        switch (op) {
        case BinaryOperator::Equal:
        case BinaryOperator::NEQ:
        case BinaryOperator::LEQ:
        case BinaryOperator::GEQ:
        case BinaryOperator::Less:
        case BinaryOperator::Greater:
          node->getType() = QualType(BoolType::get(&context.types()));
          break;
        case BinaryOperator::Plus:
        case BinaryOperator::Minus:
        case BinaryOperator::Multiply:
        case BinaryOperator::Divide:
          node->getType() = QualType(trr.first);
          break;
        default:
          break;
        }
      }
    }
    return visit;
  }
  visit_t visitIfStmt(IfStmt *node, VisitType isFirst) {
    if (isFirst == postVisit) {
      auto &condition = node->getCondition();
      auto bt = BoolType::get(&context.types());
      if (condition->getType().getType() != bt)
        condition = context.make<ImplicitCastExpr>(condition, QualType(bt));
    }
    return visit;
  }
  visit_t visitModuleDecl(ModuleDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitTagDecl(TagDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitFunctorDecl(FunctorDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitForStmt(ForStmt *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitLoopStmt(LoopStmt *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitCompoundStmt(CompoundStmt *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t add_scope(UniversalSymbolTable *node, VisitType isFirst) {
    if (isFirst)
      declContext = node;
    else
      declContext = node->getParent();
    return visit_t::visit;
  }
  ASTContext &context;
  UniversalSymbolTable *declContext { };
};
}
void Sema::inferTypes() {
  sema::TypeInferenceAST { context }.traverseModuleDecl(*context);
}
}
