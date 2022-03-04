#ifndef __IO_ASTIO_HH__
#define __IO_ASTIO_HH__

#include "AST/Include.hh"
#include <AST/RecursiveASTVisitor.hh>
#include <Common/Utility.hh>
#include <Io/IOStream.hh>
#include <sstream>
#include <stack>

namespace _astnp_ {
struct DumpType: RecursiveASTVisitor<DumpType, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>> {
public:
  DumpType(std::ostream &ost) :
      ost(ost) {
  }
  visit_t visitQualType(QualType *node, bool isFirst) {
    auto qualifiers = node->getQualifiers();
    if (isFirst)
      ost << ((qualifiers & QualType::Const) == QualType::Const ? "const " : "");
    else
      ost << ((qualifiers & QualType::Reference) == QualType::Reference ? " &" : "");
    return visit_value;
  }
  visit_t visitCType(CType *node, bool isFirst) {
    if (isFirst)
      ost << "__ctype__";
    return visit_t::skip;
  }
  visit_t visitVariadicType(VariadicType *node, bool isFirst) {
    if (isFirst) {
      if (node->getUnderlying())
        dynamicTraverse(node->getUnderlying());
      ost << "...";
    }
    return visit_t::skip;
  }
  visit_t visitUnresolvedType(UnresolvedType *node, bool isFirst) {
    if (isFirst)
      ost << "#" << node->getIdentifier() << "#";
    return visit_t::skip;
  }
  visit_t visitBuiltinType(BuiltinType *node, bool isFirst) {
    if (isFirst)
      ost << _astnp_::to_string(node->classOf());
    return visit_t::skip;
  }
  visit_t visitDefinedType(DefinedType *node, bool isFirst) {
    if (isFirst) {
      ost << _astnp_::to_string(node->classOf()) << ":" << node->getIdentifier() << " " << node->getDecl().data();
    }
    return visit_t::skip;
  }
  visit_t visitPtrType(PtrType *node, bool isFirst) {
    if (!isFirst)
      ost << "*";
    return visit_t::visit;
  }
  visit_t visitArrayType(ArrayType *node, bool isFirst) {
    if (!isFirst)
      ost << "[]";
    return visit_t::visit;
  }
private:
  std::ostream &ost;
};
struct DumpAST: RecursiveASTVisitor<DumpAST, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>, true> {
  using parent_t = RecursiveASTVisitor<DumpAST, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>, true>;
public:
  DumpAST() {
    color_stack.push(Color::Default());
  }
  visit_t visitASTNode(ASTNode *node, bool isFirst) {
    auto kind = node->classOf();
    if (isFirst) {
      if (isStmt(kind)) {
        push_color(Color::Magenta());
      } else if (isDecl(kind))
        push_color(Color::AquaGreen());
      if (!isType(kind)) {
        cst() << std::string(indent, '-') + "+" << to_string(kind) << " ";
        push_color(Color::Default());
//        cst() << node << ":" << node->parent() << " ";
        cst() << node << " ";
        extentInfo(node);
        pop_color();
        cst() << " ";
      }
      indent++;
    } else {
      indent--;
      pop_color();
      cst();
    }
    return visit_value;
  }
  visit_t visitDeclRefExpr(DeclRefExpr *node, bool isFirst) {
    if (isFirst) {
      ost << node->getIdentifier() << " ";
      if (node->getDecl())
        ost << *(node->getDecl()) << " ";
    }
    return visit_value;
  }
  visit_t visitBinaryOperation(BinaryOperation *node, bool isFirst) {
    if (isFirst)
      ost << to_string(node->getOperator()) << " ";
    return visit_value;
  }
  visit_t visitImportDecl(ImportDecl *node, bool isFirst) {
    if (isFirst)
      ost << node->getModulename() << " " << node->getModule().data() << " ";
    return visit_value;
  }
  visit_t visitNamedDecl(NamedDecl *node, bool isFirst) {
    if (isFirst)
      ost << node->getIdentifier() << " ";
    return visit_value;
  }
  visit_t visitFunctorDecl(FunctorDecl *node, bool isFirst) {
    if (isFirst) {
      auto &type = node->getReturntype();
      if (!node->getComplete()) {
        push_color(Color::Red());
        cst() << "I ";
        pop_color();
        cst();
      }
      ost << "'";
      dumpType(&type, false);
      ost << " (";
      auto &args = node->getParameters();
      for (size_t i = 0; i < args.size(); ++i) {
        dumpType(&(node->getParameters(i)->getType()), false);
        if ((i + 1) < args.size())
          ost << ", ";
      }
      ost << ")'";
    }
    return visit_value;
  }
  visit_t visitExternFunctionDecl(ExternFunctionDecl *node, bool isFirst) {
    if (isFirst) {
      auto &type = node->getReturntype();
      ost << "'";
      dumpType(&type, false);
      ost << " (";
      auto &args = node->getParameters();
      for (size_t i = 0; i < args.size(); ++i) {
        dumpType(&(node->getParameters(i)), false);
        if ((i + 1) < args.size())
          ost << ", ";
      }
      ost << ")'";
    }
    return visit_value;
  }
  visit_t visitVariableDecl(VariableDecl *node, bool isFirst) {
    if (isFirst) {
      auto &type = node->getType();
      dumpType(&type);
    }
    return visit_value;
  }
  visit_t visitUsingDecl(UsingDecl *node, bool isFirst) {
    if (isFirst) {
      auto &type = node->getType();
      dumpType(&type);
    }
    return visit_value;
  }
  visit_t visitLiteralExpr(LiteralExpr *node, bool isFirst) {
    if (isFirst)
      ost << "'" << node->getValue() << "' ";
    return visit_value;
  }
  visit_t visitQualType(QualType *node, bool isFirst) {
    return visit_t::skip;
  }
  visit_t visitType(Type *node, bool isFirst) {
    return visit_t::skip;
  }
  template <typename T>
  visit_t postWalk(T *node, bool isFirst) {
    if (isType(T::kind))
      return visit_t::skip;
    if (isFirst) {
      if (auto expr = dynamic_cast<Expr*>(node))
        dumpType(&(expr->getType()));
      ost << std::endl;
    }
    return visit_value;
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
