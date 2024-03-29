#include <deque>
#include <map>
#include <string>
#include <Sema/Sema.hh>
#include <AST/Visitors/ASTVisitor.hh>
#include <Support/Enumerate.hh>

namespace tlang {
namespace sema {
struct TypeInferenceAST: ASTVisitor<TypeInferenceAST, VisitorPattern::prePostOrder> {
  TypeInferenceAST(ASTContext &context) :
      context(context) {
  }
  visit_t visitDeclRefExpr(DeclRefExpr *node, VisitType isFirst) {
    if (isFirst == preVisit) {
      auto decl = node->getDecl().data();
      assert(decl);
      if (auto vd = dyn_cast<VariableDecl>(decl))
        node->getType() = vd->getType().addQuals(QualType::Reference);
      else if (auto fd = dyn_cast<FunctorDecl>(decl))
        node->getType() = QualType(fd->getType());
      else
        assert(false);
    }
    return visit;
  }
  visit_t visitParenExpr(ParenExpr *node, VisitType isFirst) {
    if (isFirst == postVisit)
      node->getType() = node->getExpr()->getType();
    return visit;
  }
  visit_t visitMemberExpr(MemberExpr *node, VisitType isFirst) {
    // TODO Need to resolve pointers
    if (isFirst == postVisit) {
      auto type = node->getOwner()->getType().getType();
      if (auto st = dyn_cast<StructType>(type)) {
        auto structDecl = dyn_cast<StructDecl>(st->getDecl().data());
        if (auto member = dyn_cast<DeclRefExpr>(node->getMember().data())) {
          if (auto symbol = structDecl->find(member->getIdentifier(), true)) {
            auto decl = *symbol;
            if (auto md = dyn_cast<MemberDecl>(decl)) {
              member->getType() = md->getType().addQuals(QualType::Reference);
              node->getType() = member->getType();
            } else if (auto md = dyn_cast<MethodDecl>(decl)) {
              member->getType() = QualType(md->getType());
              node->getType() = member->getType();
            }
            member->getDecl() = decl;
          } else
            throw(std::runtime_error("Invalid member reference"));
        } else
          throw(std::runtime_error("Invalid member value"));
      } else
        throw(std::runtime_error("Invalid member expression"));
    }
    return visit;
  }
  visit_t visitRangeExpr(RangeExpr *node, VisitType isFirst) {
    if (isFirst == postVisit) {
      auto startType = node->getStart()->getType();
      auto stopType = node->getStop()->getType();
      auto trr = typePromotion(startType.getType()->getCanonicalType(), stopType.getType()->getCanonicalType());
      if (!trr.first)
        throw(std::runtime_error("Invalid range expr"));
      if (trr.second == 0)
        node->getStart() = context.make<ImplicitCastExpr>(node->getStart(), stopType.addQuals());
      else if (trr.second == 1)
        node->getStop() = context.make<ImplicitCastExpr>(node->getStop(), startType.addQuals());
      if (auto &step = node->getStep()) {
        if (step->getType().getType() != trr.first)
          step = context.make<ImplicitCastExpr>(step, startType.addQuals());
      }
      node->getType() = QualType(trr.first);
    }
    return visit;
  }
  visit_t visitCallExpr(CallExpr *node, VisitType isFirst) {
    if (isFirst == postVisit) {
      if (auto ref = dyn_cast<DeclRefExpr>(node->getCallee())) {
        if (auto ft = dyn_cast<FunctionType>(ref->getType().getType())) {
          node->getType() = ft->getReturnType();
          auto &paremeters = ft->getParemeters();
          for (auto [i, arg] : tlang::enumerate(node->getArgs())) {
            auto tp = typePromotion(paremeters[i].getType(), arg->getType().getType());
            if (tp.first == nullptr)
              throw(std::runtime_error("Invalid arg"));
            if (tp.second != -1) {
              arg = context.make<ImplicitCastExpr>(arg, QualType(tp.first));
            }
          }
        }
      } else if (auto me = dyn_cast<MemberExpr>(node->getCallee())) {
        auto ref = me->getMember().data();
        if (auto member = dyn_cast<DeclRefExpr>(ref)) {
          auto uft = member->getType().getType();
          if (auto ft = dyn_cast<FunctionType>(uft))
            node->getType() = ft->getReturnType();
        }
      }
      //TODO Implicit cast to arg
    }
    return visit;
  }
  visit_t visitReduceExpr(ReduceExpr *node, VisitType isFirst) {
    if (isFirst == postVisit) {
      node->getType() = node->getExpr()->getType().modQuals();
    }
    return visit;
  }
  visit_t visitIdExpr(IdExpr *node, VisitType isFirst) {
    if (isFirst == preVisit) {
      node->getType() = context.makeType<IntType>(IntType::P_32, IntType::Signed);
    }
    return visit;
  }
  visit_t visitDimExpr(DimExpr *node, VisitType isFirst) {
    if (isFirst == preVisit) {
      node->getType() = context.makeType<IntType>(IntType::P_32, IntType::Signed);
    }
    return visit;
  }
  visit_t visitArrayExpr(ArrayExpr *node, VisitType isFirst) {
    if (isFirst == postVisit) {
      auto qualType = node->getArray()->getType();
      if (auto type = dyn_cast<ArrayType>(qualType.getType())) {
        if (type->getLayout().size() != node->getIndex().size())
          throw(std::runtime_error("Invalid index for array"));
        node->getType() = QualType((QualType::cvr_qualifiers) (QualType::Reference | qualType.getQualifiers()), 0, type->getUnderlying());
      } else if (auto type = dyn_cast<PtrType>(qualType.getType())) {
        if (node->getIndex().size() != 1)
          throw(std::runtime_error("Invalid index for array"));
        node->getType() = QualType((QualType::cvr_qualifiers) (QualType::Reference | qualType.getQualifiers()), 0, type->getUnderlying());
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
          node->getType() = QualType(QualType::Reference, 0, pt->getUnderlying());
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
  visit_t visitTernaryOperator(TernaryOperator *node, VisitType isFirst) {
    if (isFirst == postVisit) {
      auto &condition = node->getCondition();
      auto bt = BoolType::get(&context.types());
      if (condition->getType().getType() != bt)
        condition = context.make<ImplicitCastExpr>(condition, QualType(bt));
      QualType lhs = node->getLhs()->getType();
      QualType rhs = node->getRhs()->getType();
      auto trr = typePromotion(lhs.getType()->getCanonicalType(), rhs.getType()->getCanonicalType());
      if (trr.first == nullptr)
        throw(std::runtime_error("Invalid ternary operator"));
      node->getType() = QualType(lhs.isReference() && rhs.isReference() ? QualType::Reference : QualType::None, 0, trr.first);
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
        case BinaryOperator::And:
        case BinaryOperator::Or:
          node->getType() = QualType(BoolType::get(&context.types()));
          break;
        case BinaryOperator::LShift:
        case BinaryOperator::RShift:
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
  visit_t visitUnitDecl(UnitDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitVariableDecl(VariableDecl *node, VisitType isFirst) {
    if (postVisit == isFirst)
      if (node->getInit()) {
        auto lhs = node->getType().getType()->getCanonicalType().data();
        auto rhs = node->getInit()->getType().getType()->getCanonicalType().data();
        if (lhs != rhs)
          node->getInit() = context.make<ImplicitCastExpr>(node->getInit(), node->getType().modQuals());
      }
    return visit;
  }
  visit_t visitModuleDecl(ModuleDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitTagDecl(TagDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitFunctorDecl(FunctorDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitForStmt(ForStmt *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitLoopStmt(LoopStmt *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitCompoundStmt(CompoundStmt *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t add_scope(UniversalContext *node, VisitType isFirst) {
    if (isFirst)
      declContext = node;
    else
      declContext = node->getParent();
    return visit_t::visit;
  }
  ASTContext &context;
  UniversalContext *declContext { };
};
}
void Sema::inferTypes() {
  sema::TypeInferenceAST { *context }.traverseUniverseDecl(**context);
}
}
