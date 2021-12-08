#include <io.hh>
#include <macros.hh>
#include <lexer.hh>
#include <parser.hh>
#include <recursive_ast_visitor.hh>

struct Visitor: _astnp_::RecursiveASTVistor<Visitor> {
public:
  Visitor() {
  }
  bool visitASTNode(_astnp_::ASTNode *node) {
    std::cerr << std::string(ident, '-') + _astnp_::to_string(node->classOf());
    ident++;
    return true;
  }
  bool visitNamedDecl(_astnp_::NamedDecl *node) {
    std::cerr << ": " << node->getIdentifier();
    return true;
  }
  bool postVisit(_astnp_::ASTNode *node) {
    std::cerr << std::endl;
    return true;
  }
  bool afterVisit(_astnp_::ASTNode *node) {
    ident--;
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
  Visitor v;
  ASTContext context = parser.parse();
  v.traverseModuleDecl(*context);
  return 0;
}
