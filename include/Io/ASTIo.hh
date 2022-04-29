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
#include <Io/StringEmitter.hh>

namespace tlang {
namespace io {
struct DumpType: public StringEmitterVisitor<DumpType> {
  std::string emitBoolType(BoolType *type) {
    return "bool";
  }
  std::string emitAddressType(AddressType *type) {
    return "address";
  }
  std::string emitIntType(IntType *type) {
    switch (type->getPrecision()) {
    case IntType::P_8:
      return type->isSigned() ? "i8" : "u8";
    case IntType::P_16:
      return type->isSigned() ? "i16" : "u16";
    case IntType::P_32:
      return type->isSigned() ? "i32" : "u32";
    case IntType::P_64:
      return type->isSigned() ? "i64" : "u64";
    default:
      return type->isSigned() ? "int" : "uint";
    }
  }
  std::string emitFloatType(FloatType *type) {
    switch (type->getPrecision()) {
    case FloatType::P_8:
      return "f8";
    case FloatType::P_16:
      return "f16";
    case FloatType::P_32:
      return "f32";
    case FloatType::P_64:
      return "f64";
    default:
      return "float";
    }
  }
  std::string emitVariadicType(VariadicType *type) {
    return "...";
  }
  std::string emitUnresolvedType(UnresolvedType *node) {
    return frmt("#{0}#", node->getIdentifier());
  }
  std::string emitDefinedType(DefinedType *node) {
    assert(node->getDecl());
    return frmt("D! {0}({1})#", node->getDecl()->getIdentifier(), static_cast<void*>(node->getDecl().data()));
  }
  std::string emitPtrType(PtrType *type) {
    return frmt("{0}*", emitType(type->getUnderlying()));
  }
  std::string emitFunctionType(FunctionType *node) {
    std::string value = frmt("F! {0}(", emitQualType(node->getReturnType()));
    auto &args = node->getParemeters();
    for (auto [i, arg] : tlang::enumerate(args)) {
      value += emitQualType(arg);
      if (i + 1 < args.size())
        value += ", ";
    }
    return value + ")";
  }
  std::string visitArrayType(ArrayType *node) {
    return frmt("{0}[]", emitType(node->getUnderlying()));
  }
  std::string emitQualType(QualType node) {
    auto qualifiers = node.getQualifiers();
    auto preQual = (qualifiers & QualType::Const) == QualType::Const ? "const " : "";
    auto type = node.getType() ? emitType(node.getType()) : "void";
    auto postQual = (qualifiers & QualType::Reference) == QualType::Reference ? "&" : "";
    auto extraInfo = node.getAddressSpace() ? frmt(" <{0}>", node.getAddressSpace()) : std::string();
    return frmt("{0}{1}{2}{3}", preQual, type, postQual, extraInfo);
  }
};
}

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
    ost << io::DumpType { }.emitQualType(*node);
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
