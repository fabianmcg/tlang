#ifndef __AST_NODE_HH__
#define __AST_NODE_HH__

#include <cstdint>
#include <macros.hh>
#include <extent.hh>
#include <node_kind.hh>
#include <memory>

namespace __lang_np__ {
class ASTNode {
public:
  ASTNode(int64_t id, const Extent &extent) :
      __extent(extent), __id(id) {
  }
  ASTNode(int64_t id, const location &start, const location &end = location { }) :
      __extent(Extent { start, end }), __id(id) {
  }
  ASTNode() = delete;
  ASTNode(ASTNode &&other) {
    __extent = std::exchange(other.__extent, Extent { });
    __id = std::exchange(other.__id, -1);
    __parent = std::exchange(other.__parent, nullptr);
  }
  ASTNode(const ASTNode&) = delete;
  virtual ~ASTNode() = default;
  auto parent() {
    return __parent;
  }
  auto parent() const {
    return __parent;
  }
  auto id() const {
    return __id;
  }
  auto extent() const {
    return __extent;
  }
  auto beginLocation() const {
    return __extent.begin;
  }
  auto endLocation() const {
    return __extent.end;
  }
  virtual bool valid() const {
    return __id != -1;
  }
  virtual ASTNodeKind kind() const {
    return ASTNodeKind::ASTNode;
  }
  virtual std::string to_string() const {
    return _astnp_::to_string(kind()) + __extent.to_string();
  }
protected:
  Extent __extent { };
  int64_t __id { -1 };
  ASTNode *__parent { };
};
inline std::ostream& operator<<(std::ostream &ost, const ASTNode &node) {
  ost << node.to_string();
  return ost;
}
}
#endif
