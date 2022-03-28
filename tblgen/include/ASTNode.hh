#ifndef __AST_NODE_HH__
#define __AST_NODE_HH__

#include "Infra/AbstractNode.hh"

class ASTNode: public AbstractNode {
public:
  ASTNode(llvm::Record &record, bool analysis = true) :
      AbstractNode(record) {
    if (analysis)
      analyze();
  }
  std::vector<llvm::Record*>& parents() {
    return parentNodes;
  }
  const std::vector<llvm::Record*>& parents() const {
    return parentNodes;
  }
  std::vector<Child>& children() {
    return childrenNodes;
  }
  const std::vector<Child>& children() const {
    return childrenNodes;
  }
  std::vector<Variable>& members() {
    return variables;
  }
  const std::vector<Variable>& members() const {
    return variables;
  }
  std::vector<ClassSection>& sections() {
    return codeSections;
  }
  const std::vector<ClassSection>& sections() const {
    return codeSections;
  }
  std::vector<Enum>& enums() {
    return enumerations;
  }
  const std::vector<Enum>& enums() const {
    return enumerations;
  }
protected:
  std::vector<llvm::Record*> parentNodes;
  std::vector<Child> childrenNodes;
  std::vector<Variable> variables;
  std::vector<ClassSection> codeSections;
  std::vector<Enum> enumerations;
  void analyze();
};
#endif
