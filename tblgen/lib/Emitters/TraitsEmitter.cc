#include <filesystem>
#include <Utility.hh>
#include <ASTNode.hh>
#include <AST.hh>
#include <TGUtility.hh>
#include <Emitters/TraitsEmitter.hh>

namespace {
void emitTraits(AST<AbstractNode> &ast, llvm::raw_ostream &ost) {
  auto printer = [&ost](auto &node) {
    AbstractNode &an = **node;
    ost << "\n\n";
    ost << "template <> struct ASTTraits<" << an->getName() << "> {\n";
    ost << "static inline constexpr bool valid = true;";
    ost << "static constexpr ASTKind first = ASTKind::" << an->getName() << ";\n";
    if (node->children().size())
      ost << "static constexpr ASTKind last = ASTKind::last" << an->getName() << ";\n";
    else
      ost << "static constexpr ASTKind last = ASTKind::" << an->getName() << ";\n";
    ost << "static inline constexpr bool is(ASTKind kind) {";
    ost << "if constexpr (first != last) return first <= kind && kind < last;";
    ost << "else return first == kind;";
    ost << "}\n";
    ost << "static inline constexpr bool isNot(ASTKind kind) { return !is(kind); }";
    ost << "};\n";
  };
  ost << "template <typename T> struct ASTTraits {";
  ost << "static inline constexpr bool valid = false;";
  ost << "static inline constexpr bool is(ASTKind kind) { return false; }";
  ost << "static inline constexpr bool isNot(ASTKind kind) { return true; }";
  ost << "};\n";
  ast.bfs(printer);
  ost << "\n";
  auto traits =
      R""""(
template <typename T> using ASTTraits_t = ASTTraits<std::decay_t<T>>;
template <typename T> inline constexpr bool isValidNode_v = ASTTraits_t<T>::valid;
template <typename S, typename T> inline constexpr bool isASTKind_v = ASTTraits_t<S>::is(T::kind);
template <typename S, typename T> inline bool isa(T* node) {
  if constexpr (isValidNode_v<T>) 
    return ASTTraits_t<S>::is(node->classof());
  return false;
}
template <typename S, typename T, std::enable_if_t<!std::is_pointer_v<std::decay_t<T>>, int> = 0>
inline bool isa(T node) {
  if constexpr (isValidNode_v<T>) 
    return ASTTraits_t<S>::is(node.classof());
  return false;
}
template <typename S, typename T>
inline S* dyn_cast(T* node) {
  if constexpr (isValidNode_v<S> && isValidNode_v<T> && isASTKind_v<T, S>)
  if (isa<S>(node)) 
    return static_cast<S*>(node);
  return nullptr;
}
template <typename S, typename T>
inline const S* dyn_cast(const T* node) {
  if constexpr (isValidNode_v<S> && isValidNode_v<T> && isASTKind_v<T, S>)
  if (isa<S>(node)) 
    return static_cast<const S*>(node);
  return nullptr;
}
)"""";
  ost << traits;
}
}

void TraitsEmitter::run(llvm::raw_ostream &ost) {
  std::filesystem::path path = std::filesystem::path(records.getInputFilename());
  std::string fn = path.stem().string();
  llvm::emitSourceFileHeader("AST Traits", ost);
  AST<AbstractNode> ast(records);
  defineGuards(ost, "AST_TRAITS_HH", true);
  ost << "\n";
  include(ost, "type_traits", true);
  include(ost, "Common.hh", false);
  printPrologueSections(ost, records, true);
  ost << "\n";
  ost << "namespace tlang {\n";
  emitTraits(ast, ost);
  ost << "}";
  printEpilogueSections(ost, records, true);
  defineGuards(ost, "AST_TRAITS_HH", false);
}
