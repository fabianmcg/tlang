#ifndef CODEGEN_CXXEMITTER_HH
#define CODEGEN_CXXEMITTER_HH

#include <AST/Visitors/ASTVisitor.hh>
#include <llvm/Support/raw_ostream.h>

namespace tlang {
class CXXEmitter {
private:
  struct Visitor: public ASTVisitor<Visitor, VisitorPattern::preOrder> {
    Visitor(llvm::raw_ostream &ost) :
        ost(ost) {
    }
    visit_t visitCXXDecl(CXXDecl *decl) {
      ost << decl->getSource() << "\n";
      return visit;
    }
    llvm::raw_ostream &ost;
  };
public:
  void emitModuleDecl(ModuleDecl *module, llvm::raw_ostream &ost) {
    Visitor { ost }.traverseModuleDecl(module);
  }
};
}

#endif
