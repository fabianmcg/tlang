#include <Common/Io.hh>
#include <Lexer/Lexer.hh>
#include <Parser/Parser.hh>
#include "AST/RecursiveASTVisitor.hh"

struct Visitor: _astnp_::RecursiveASTVistor<Visitor, _astnp_::VisitorPattern::prePostOrder> {
public:
  Visitor() {
  }
  bool visitASTNode(_astnp_::ASTNode *node, bool isFirst) {
    if (node->is(tlang::NodeClass::ASTNodeList))
      return true;
    if (isFirst) {
      std::cerr << std::string(ident, '-') + _astnp_::to_string(node->classOf());
      if (!dynamic_cast<tlang::NamedDecl*>(node) && node->isNot(tlang::NodeClass::QualType))
        std::cerr << std::endl;
      ident++;
    } else {
      ident--;
    }
    return true;
  }
  bool visitNamedDecl(_astnp_::NamedDecl *node, bool isFirst) {
    if (isFirst)
      std::cerr << ": " << node->getIdentifier() << std::endl;
    return true;
  }
  bool visitQualType(_astnp_::QualType *node, bool isFirst) {
    if (isFirst)
      std::cerr << ": " << node->getQualifiers() << std::endl;
    return true;
  }
  size_t ident { };
};

int main(int argc, char **argv) {
  using namespace _lnp_;
  using namespace _pnp_;
  using namespace _astnp_;
  auto fs = unique_fstream::open_istream("main.tt");
  Lexer lex(*fs);
  Parser parser = Parser(lex);
  ASTContext context = parser.parse();
  Visitor v;
  v.traverseModuleDecl(*context);
  return 0;
}
