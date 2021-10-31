#ifndef __AST_NODE_KIND_HH__
#define __AST_NODE_KIND_HH__

#include <macros.hh>

namespace __lang_np__ {
enum class ASTNodeKind {
  ASTNode = 0
};
inline std::string to_string(const ASTNodeKind &kind) {
  switch (kind) {
  case ASTNodeKind::ASTNode:
    return "ASTNode";
  default:
    return "";
  }
}
inline std::ostream& operator<<(std::ostream &ost, const ASTNodeKind &kind) {
  ost << to_string(kind);
  return ost;
}
}
#endif
