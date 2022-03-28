#include <filesystem>
#include <ASTNode.hh>
#include <AST.hh>
#include <Utility.hh>
#include <Constants.hh>
#include <TGUtility.hh>
#include <Emitters/CommonEmitter.hh>

namespace {
void emitEnum(AST<AbstractNode> &ast, llvm::raw_ostream &ost) {
  auto pre = [&ost](auto &node) {
    AbstractNode &an = *node;
    ost << an->getName() << ",\n";
  };
  auto post = [&ost](auto &node) {
    AbstractNode &an = *node;
    if (node.children().size())
      ost << "last" << an->getName() << ",\n";
  };
  frmts(ost, "enum class {} {{\n", C::kind_v);
  ast.bfsRecursive(pre, post);
  ost << "};\n";
  ost << frmt("std::string to_string({} kind);\n", C::kind_v);
  ost << frmt("std::ostream& operator<<(std::ostream& ost, const {}& kind);\n", C::kind_v);
}
void emitForward(AST<AbstractNode> &ast, llvm::raw_ostream &ost) {
  auto visitor = [&ost](auto &node) {
    AbstractNode &an = **node;
    ost << "class " << an->getName() << ";\n";
  };
  ast.bfs(visitor);
}
}

void CommonEmitter::run(llvm::raw_ostream &ost) {
  std::filesystem::path path = std::filesystem::path(records.getInputFilename());
  std::string fn = path.stem().string();
  if (mode == EmitMode::header) {
    llvm::emitSourceFileHeader("AST Common header", ost);
    AST<AbstractNode> ast(records);
    defineGuards(ost, "AST_COMMON_HH", true);
    include(ost, "iostream", true);
    include(ost, "string", true);
    printPrologueSections(ost, records, true);
    frmts(ost, "namespace {} {{\n", C::namespace_v);
    emitEnum(ast, ost);
    emitForward(ast, ost);
    ost << "}";
    printEpilogueSections(ost, records, true);
    defineGuards(ost, "AST_COMMON_HH", false);
  } else {
    include(ost, "AST/Common.hh", true);
    printPrologueSections(ost, records, false);
    frmts(ost, "namespace {} {{\n", C::namespace_v);
    auto to_string_m = R""""(
std::string to_string({0} kind) {{
  switch(kind) {{
#define AST_MACRO(BASE, PARENT) case {0}::BASE: return #BASE;
#include "AST/Nodes.inc"
  }}
  return "";
}}
)"""";
    frmts(ost, to_string_m, C::kind_v);
    frmts(ost, "std::ostream& operator<<(std::ostream& ost, const {}& kind){{ ost << to_string(kind); return ost; }}\n", C::kind_v);
    ost << "}";
    printEpilogueSections(ost, records, false);
  }
}
