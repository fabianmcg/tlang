#ifndef __IO_ASTIO_HH__
#define __IO_ASTIO_HH__

#include <sstream>
#include <stack>
#include "AST/Attr.hh"
#include "AST/Decl.hh"
#include "AST/Expr.hh"
#include "AST/Type.hh"
#include "AST/Stmt.hh"
#include <AST/Visitors/ASTVisitor.hh>
#include <Io/IOStream.hh>

namespace tlang {
struct DumpType: public ASTVisitor<DumpType, VisitorPattern::prePostOrder> {
public:
  DumpType(std::ostream &ost) :
      ost(ost) {
  }
  visit_t visitQualType(QualType *node, VisitType kind) {
    auto qualifiers = node->getQualifiers();
    if (kind)
      ost << ((qualifiers & QualType::Const) == QualType::Const ? "const " : "");
    else
      ost << ((qualifiers & QualType::Reference) == QualType::Reference ? " &" : "") << " [" << node->getType() << "]{"
          << node->getAddressSpace() << "}";
    return visit;
  }
  visit_t visitFunctionType(FunctionType *node, VisitType kind) {
    if (kind) {
      ost << "F! ";
      traverseQualType(&(node->getReturnType()));
      ost << "(";
      auto &args = node->getParemeters();
      for (auto &arg : args) {
        traverseQualType(&arg);
        ost << ",";
      }
      ost << ")";
    }
    return visit_t::skip;
  }
  visit_t visitVariadicType(VariadicType *node, VisitType kind) {
    if (kind) {
      if (node->getUnderlying())
        dynamicTraverse(node->getUnderlying());
      ost << "...";
    }
    return visit_t::skip;
  }
  visit_t visitUnresolvedType(UnresolvedType *node, VisitType kind) {
    if (kind)
      ost << "#" << node->getIdentifier() << "#";
    return visit_t::skip;
  }
  visit_t visitBuiltinType(BuiltinType *node, VisitType kind) {
    if (kind)
      ost << tlang::to_string(node->classof());
    return visit;
  }
  visit_t visitPtrType(PtrType *node, VisitType kind) {
    if (!kind)
      ost << "*";
    return visit_t::visit;
  }
  visit_t visitDefinedType(DefinedType *node, VisitType kind) {
    if (kind) {
      std::string id { };
      if (node->getDecl())
        id = node->getDecl()->getIdentifier();
      ost << "S!" << id << " " << "(" << node->getDecl().data() << ")";
    }
    return visit_t::visit;
  }
  visit_t visitArrayType(ArrayType *node, VisitType kind) {
    if (!kind)
      ost << "[]";
    return visit_t::visit;
  }
private:
  std::ostream &ost;
};
struct DumpAST: ASTVisitor<DumpAST, VisitorPattern::prePostOrder | VisitorPattern::postWalk> {
public:
  DumpAST() {
    color_stack.push(Color::Default());
  }
  visit_t visitASTNode(ASTNode *node, VisitType kind) {
    auto classof = node->classof();
    if (kind) {
      if (Stmt::classof(classof)) {
        push_color(Color::Magenta());
      } else if (Decl::classof(classof))
        push_color(Color::AquaGreen());
      if (!ProtoType::classof(classof)) {
        cst() << std::string(indent, '-') + "+" << to_string(classof) << " ";
        push_color(Color::Default());
//        cst() << node << ":" << node->parent() << " ";
        cst() << node << " ";
        extentInfo(node);
        pop_color();
        cst() << " ";
        indent++;
      }
    } else {
      if (!ProtoType::classof(classof))
        indent--;
      pop_color();
      cst();
    }
    return visit;
  }
  visit_t visitDeclRefExpr(DeclRefExpr *node, VisitType kind) {
    if (kind) {
      ost << node->getIdentifier() << " ";
      if (node->getDecl())
        ost << node->getDecl().data() << " ";
    }
    return visit;
  }
  visit_t visitMemberExpr(MemberExpr *node, VisitType kind) {
    if (kind) {
      if (auto drf = dyn_cast<DeclRefExpr>(node->getMember().data())) {
        ost << "." << drf->getIdentifier();
      }
    }
    return visit;
  }
//  visit_t visitBinaryOperation(BinaryOperator *node, VisitType kind) {
//    if (kind)
//      ost << to_string(node->getOperator()) << " ";
//    return visit;
//  }
  visit_t visitImportDecl(ImportDecl *node, VisitType kind) {
    if (kind)
      ost << node->getModuleName() << " " << node->getModule().data() << " ";
    return visit;
  }
  visit_t visitNamedDecl(NamedDecl *node, VisitType kind) {
    if (kind)
      ost << node->getIdentifier() << " ";
    return visit;
  }
  visit_t visitFunctorDecl(FunctorDecl *node, VisitType kind) {
    if (kind) {
      if (!node->getComplete()) {
        push_color(Color::Red());
        cst() << "I ";
        pop_color();
        cst();
      }
      ost << "'";
      dumpType(&node->getType(), false);
    }
    return visit;
  }
  visit_t visitVariableDecl(VariableDecl *node, VisitType kind) {
    if (kind) {
      auto &type = node->getType();
      dumpType(&type);
    }
    return visit;
  }
  visit_t visitUsingDecl(UsingDecl *node, VisitType kind) {
    if (kind) {
      auto &type = node->getType();
      dumpType(&type);
    }
    return visit;
  }
  visit_t visitLiteralExpr(LiteralExpr *node, VisitType kind) {
    if (kind)
      ost << "'" << node->getValue()->to_string() << "' ";
    return visit;
  }
  visit_t visitParallelConstruct(ParallelConstruct *node, VisitType kind) {
    if (kind) {
      ost << "<" << node->getContext().data() << "> ";
    }
    return visit;
  }
  visit_t visitParallelStmtOptions(ParallelStmtOptions *node, VisitType kind) {
    if (kind) {
      ost << (node->getNoWait() ? "nowait" : "wait") << " ";
    }
    return visit;
  }
  visit_t visitLoopStmt(LoopStmt *node, VisitType kind) {
    if (kind) {
      ost << "M: " << node->getMapping() << " ";
    }
    return visit;
  }
  visit_t visitContextStmt(ContextStmt *node, VisitType kind) {
    std::string ck { };
    switch (node->getContextKind()) {
    case ContextStmt::Inherited:
      ck = "inherited";
      break;
    case ContextStmt::Default:
      ck = "default";
      break;
    case ContextStmt::Device:
      ck = "device";
      break;
    case ContextStmt::Host:
      ck = "host";
      break;
    }
    if (kind)
      ost << "[" << ck << "]";
    return visit_t::visit;
  }
  visit_t visitQualType(QualType *node, VisitType kind) {
    return visit_t::skip;
  }
  visit_t visitType(Type *node, VisitType kind) {
    return visit_t::skip;
  }
  template <typename T>
  visit_t postWalk(T *node, VisitType kind) {
    if (ProtoType::classof(T::kind))
      return visit_t::skip;
    if (kind == preVisit) {
      if (auto expr = dynamic_cast<Expr*>(node))
        dumpType(&(expr->getType()));
      ost << std::endl;
    }
    return visit;
  }
  std::string str() const {
    return Color::Default_code + ost.str() + Color::Default_code;
  }
private:
  std::ostringstream ost { };
  size_t indent { };
  std::stack<Color> color_stack { };
  void push_color(Color color) {
    color_stack.push(color);
  }
  void pop_color() {
    if (color_stack.size() > 1)
      color_stack.pop();
  }
  std::ostream& cst() {
    ost << color_stack.top();
    return ost;
  }
  void dumpType(QualType *node, bool quotes = true) {
    push_color(Color::Green());
    if (quotes)
      cst() << "'";
    else
      cst();
    DumpType { ost }.traverseQualType(node);
    if (quotes)
      ost << "'";
    pop_color();
  }
  void extentInfo(ASTNode *node) {
    ost << "<";
    push_color(Color::Orange());
    auto range = node->getSourceRange();
    cst() << range.begin.str() << " " << range.end.str();
    pop_color();
    cst() << ">";
  }
};
template <typename T>
void dump(T *node, std::ostream &ost = std::cout) {
  DumpAST visitor { };
  visitor.dynamicTraverse(node);
  ost << visitor.str();
}
}
#endif
