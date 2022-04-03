#include <filesystem>
#include <ASTNode.hh>
#include <AST.hh>
#include <Utility.hh>
#include <Constants.hh>
#include <Emitters/CodeGenEmitter.hh>

namespace {
struct ExtraInfo {
  llvm::StringRef type;
};
using TypeMap = std::map<TreeNode<AbstractNode>*, ExtraInfo>;
void visit(TreeNode<AbstractNode> &treeNode, llvm::raw_ostream &ost, TypeMap &map) {
  AbstractNode &node = *treeNode;
  if (node.abstract() || !node.emittable())
    return;
  frmts(ost, "template <> CodeGenTraits<{}>{{ using type = {}; }};", node.name(), map.at(&treeNode).type.str());
}
void emitCodeGen(AST<AbstractNode> &ast, llvm::raw_ostream &ost, TypeMap &map) {
  ost
      << R""""(
namespace codegen {
template <typename T>
CodeGenTraits {
  using type = llvm::Value*;
};
)"""";
  {
    auto visitor = [&ost, &map](auto &node) {
      visit(*node, ost, map);
    };
    ast.bfs(visitor);
  }
  ost << "template <typename T> using CodeGenTraits_t = typename CodeGenTraits<T>::type;" ;
  ost << "}\n";
}
void emitCodeGen(AST<AbstractNode> &ast, llvm::raw_ostream &ost) {
  std::string defaultType = "llvm::Value*";
  TypeMap typeMap;
  typeMap[nullptr] = ExtraInfo { llvm::StringRef(defaultType) };
  auto visitor = [&typeMap, &defaultType](TreeNode<AbstractNode> *node) {
    if ((**node).unsetIrType())
      typeMap[node] = typeMap[node->parent()];
    else
      typeMap[node] = ExtraInfo { (**node).irType() };
  };
  ast.bfs(visitor);
  emitCodeGen(ast, ost, typeMap);
}
}

void CodeGenEmitter::run(llvm::raw_ostream &ost) {
  std::filesystem::path path = std::filesystem::path(records.getInputFilename());
  std::string fn = path.stem().string();
  AST<AbstractNode> ast(records);
  if (mode == EmitMode::header) {
    llvm::emitSourceFileHeader("CodeGen llvm type traits", ost);
    defineGuards(ost, "CODEGEN_TRAITS_HH", true);
    include(ost, "AST/Common.hh", true);
    include(ost, "LLVMFwdTypes.hh", false);
    printPrologueSections(ost, records, true);
    frmts(ost, "namespace {} {{", C::namespace_v);
    emitCodeGen(ast, ost);
    ost << "}";
    printEpilogueSections(ost, records, true);
    defineGuards(ost, "CODEGEN_TRAITS_HH", false);
  }
}
