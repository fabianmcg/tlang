#include <filesystem>
#include <ASTNode.hh>
#include <AST.hh>
#include <Utility.hh>
#include <Emitters/IncFileEmitter.hh>

namespace {
inline void defineMacro(llvm::raw_ostream &ost, const std::string &macro, const std::string &body = "") {
  ost << "\n#ifndef " << macro << "\n";
  ost << "#define " << macro << body << "\n";
  ost << "#endif" << "\n";
}
inline void undefineMacro(llvm::raw_ostream &ost, const std::string &macro) {
  ost << "\n#if  defined(" << macro << ")\n";
  ost << "#undef " << macro << "\n";
  ost << "#endif" << "\n";
}
inline void preVisit(llvm::raw_ostream &ost, TreeNode<AbstractNode> &node) {
  std::string macro = toupper((*node)->getName().str());
  ost << "\n#ifndef " << macro << "\n";
  ost << "#define " << macro << "(NODE, BASE) ";
  if (node->top()) {
    ost << "AST_MACRO(NODE, BASE)";
  } else if (node->hasBase()) {
    auto base = node->base();
    ost << toupper(base->getName().str()) << "(NODE, BASE)";
  } else
    ost << "AST_MACRO(NODE, BASE)";
  ost << "\n";
  ost << "#endif" << "\n";
  if (node->hasBase()) {
    auto base = node->base();
    ost << macro << "(" << (*node)->getName() << ", " << base->getName() << ")\n";
  } else
    ost << macro << "(" << (*node)->getName() << ", NO_PARENT)\n";
}
inline void postVisit(llvm::raw_ostream &ost, TreeNode<AbstractNode> &node) {
  std::string macro = toupper((*node)->getName().str());
  ost << "#undef " << macro << "\n";
}
}

void IncFileEmitter::run(llvm::raw_ostream &ost) {
  std::filesystem::path path = std::filesystem::path(records.getInputFilename());
  std::string fn = path.stem().string();
  llvm::emitSourceFileHeader("INCLUDE NODES", ost);
  defineMacro(ost, "NO_PARENT", "");
  defineMacro(ost, "AST_MACRO", "(NODE, BASE)");
  AST<AbstractNode> ast(records);
  auto pre = [&ost](auto &node) {
    preVisit(ost, node);
  };
  auto post = [&ost](auto &node) {
    postVisit(ost, node);
  };
  ast.bfsRecursive(pre, post);
  undefineMacro(ost, "AST_MACRO");
  undefineMacro(ost, "NO_PARENT");
}
