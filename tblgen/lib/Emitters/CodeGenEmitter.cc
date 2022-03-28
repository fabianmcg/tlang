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
void visit(TreeNode<AbstractNode> &treeNode, llvm::raw_ostream &ost, bool withVArgs, TypeMap &map) {
  AbstractNode &node = *treeNode;
  if (node.abstract() || !node.emittable())
    return;
  if (withVArgs)
    frmts(ost, "template <typename...Args> {} emit{}({}* node, Args&&...args) {{ return nullptr; }}", map.at(&treeNode).type.str(),
        capitalize(node.name()), node.name());
  else
    frmts(ost, "{} emit{}({}* node) {{ return nullptr; }}", map.at(&treeNode).type.str(), capitalize(node.name()), node.name());
}
void walkup(TreeNode<AbstractNode> &treeNode, llvm::raw_ostream &ost, bool withVArgs, TypeMap &map) {
  AbstractNode &node = *treeNode;
  if (node.abstract() || !node.emittable())
    return;
  if (withVArgs)
    frmts(ost,
        "template <typename...Args> inline {0} walkup{1}({2}* node, Args&&...args) {{ return getDerived().emit{1}(node, std::forward<Args>(args)...); }}",
        map.at(&treeNode).type.str(), capitalize(node.name()), node.name());
  else
    frmts(ost, "inline {0} walkup{1}({2}* node) {{ return getDerived().emit{1}(node); }}", map.at(&treeNode).type.str(),
        capitalize(node.name()), node.name());
}
void traverse(TreeNode<AbstractNode> &treeNode, llvm::raw_ostream &ost, bool withVArgs, TypeMap &map) {
  AbstractNode &node = *treeNode;
  if (treeNode.children().empty()) {
    if (withVArgs)
      frmts(ost,
          "template <typename...Args> inline {0} traverse{1}({2}* node) {{ return getDerived().walkup{1}(node, std::forward<Args>(args)...); }}\n",
          map.at(&treeNode).type.str(), capitalize(node.name()), node.name());
    else
      frmts(ost, "inline {0} traverse{1}({2}* node) {{ return getDerived().walkup{1}(node); }}\n", map.at(&treeNode).type.str(),
          capitalize(node.name()), node.name());
    return;
  }
  if (withVArgs) {
    frmts(ost, "template <typename...Args> inline {0} traverse{1}({2}* node, Args&&...args) {{ switch(classof(node)) {{",
        map.at(&treeNode).type.str(), capitalize(node.name()), node.name());
    AST<AbstractNode>::bfs(
        [&ost, &treeNode](auto &node) {
          if (node == &treeNode || (**node).abstract() || !(**node).emittable())
            return;
          frmts(ost, "case {}::{}: return getDerived().walkup{}(node, std::forward<Args>(args)...);\n", C::kind_v, (**node).name(),
              capitalize((**node).name()));
        }, &treeNode);
  } else {
    frmts(ost, "inline {0} traverse{1}({2}* node) {{ switch(classof(node)) {{", map.at(&treeNode).type.str(), capitalize(node.name()),
        node.name());
    AST<AbstractNode>::bfs([&ost, &treeNode](auto &node) {
      if (node == &treeNode || (**node).abstract() || !(**node).emittable())
        return;
      frmts(ost, "case {}::{}: return getDerived().walkup{}(node);\n", C::kind_v, (**node).name(), capitalize((**node).name()));
    }, &treeNode);
  }
  ost << "default: return nullptr; }\n}\n";
}
void emitCodeGen(AST<AbstractNode> &ast, llvm::raw_ostream &ost, bool withVArgs, TypeMap &map) {
  ost
      << R""""(
template <typename Derived>
class CodeGenVisitor<Derived> {
public:
  Derived &getDerived() { return *static_cast<Derived *>(this); }
)"""";
  {
    auto visitor = [&ost, &withVArgs, &map](auto &node) {
      visit(*node, ost, withVArgs, map);
    };
    ast.bfs(visitor);
  }
  {
    auto visitor = [&ost, &withVArgs, &map](auto &node) {
      walkup(*node, ost, withVArgs, map);
    };
    ast.bfs(visitor);
  }
  {
    auto visitor = [&ost, &withVArgs, &map](auto &node) {
      traverse(*node, ost, withVArgs, map);
    };
    ast.bfs(visitor);
  }
  ost << "};\n";
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
  emitCodeGen(ast, ost, false, typeMap);
  emitCodeGen(ast, ost, true, typeMap);
}
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
void emittable(AST<AbstractNode> &ast, llvm::raw_ostream &ost) {
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
  auto preVisitor = [&ost, &typeMap](TreeNode<AbstractNode> &treeNode) {
    AbstractNode &node = *treeNode;
    if (node.abstract() || !node.emittable())
      return;
    defineMacro(ost, toupper(node.name()),
        frmt(" {} emit{}({}* node);", typeMap.at(&treeNode).type.str(), capitalize(node.name()), node.name()));
    ost << toupper(node.name()) << "\n";
  };
  auto postVisitor = [&ost](TreeNode<AbstractNode> &treeNode) {
    AbstractNode &node = *treeNode;;
    if (node.abstract() || !node.emittable())
      return;
    undefineMacro(ost, toupper(node.name()));
  };
  ast.bfsRecursive(preVisitor, postVisitor);
}
}

void CodeGenEmitter::run(llvm::raw_ostream &ost) {
  std::filesystem::path path = std::filesystem::path(records.getInputFilename());
  std::string fn = path.stem().string();
  AST<AbstractNode> ast(records);
  if (mode == EmitMode::header) {
    llvm::emitSourceFileHeader("CodeGen visitor template", ost);
    defineGuards(ost, "CODEGEN_RECURSIVEVISITOR_HH", true);
    include(ost, "AST/Common.hh", true);
    printPrologueSections(ost, records, true);
    frmts(ost, "namespace {} {{\n", C::namespace_v);
    emitCodeGen(ast, ost);
    ost << "}";
    printEpilogueSections(ost, records, true);
    defineGuards(ost, "CODEGEN_RECURSIVEVISITOR_HH", false);
  } else {
    llvm::emitSourceFileHeader("CodeGen emittable nodes inc", ost);
    emittable(ast, ost);
  }
}
