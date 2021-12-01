#ifndef __AST_NODE_HH__
#define __AST_NODE_HH__

#include <cstdint>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>

#include "macros.hh"
#include "source_range.hh"
#include "nodes.hh"
#include "decl_context.hh"

namespace _astnp_ {
template <typename ...T>
struct parent_container {
  static constexpr size_t size = sizeof...(T);
  using parent_types_t = std::tuple<T...>;
  template <int I>
  using type_t = std::tuple_element_t<I, parent_types_t>;
};
template <>
struct parent_container<> {
  static constexpr size_t size = 0;
  template <int I>
  using type_t = void;
};
enum class children_kind {
  static_node,
  dynamic_node,
  dynamic_list
};
template <children_kind K, typename T, int O, bool V = true>
struct children_node {
  static constexpr children_kind kind = K;
  using type = T;
  static constexpr int offset = O;
  static constexpr bool visit = V;
};
class ASTNode;
template <typename ...T>
class children_container;
template <>
class children_container<> {
public:
  static constexpr bool is_trivial = true;
  static constexpr size_t size = 0;
  ASTNode* begin() const {
    return nullptr;
  }
  ASTNode* end() const {
    return nullptr;
  }
};
class ASTNode {
public:
  using parents_t = parent_container<>;
  using node_kind_t = NodeClass;
  static constexpr node_kind_t kind = node_kind_t::ASTNode;
  virtual node_kind_t classOf() const {
    return kind;
  }
  using children_t = children_container<>;
  ASTNode() = default;
  ASTNode(const SourceRange &range) :
      __range(range) {
  }
  ASTNode(const SourceLocation &start, const SourceLocation &end = SourceLocation { }) :
      __range(SourceRange { start, end }) {
  }
  ASTNode(ASTNode &&other) {
    __range = std::exchange(other.__range, SourceRange { });
    __parent = std::exchange(other.__parent, nullptr);
  }
  ASTNode(const ASTNode&) = delete;
  virtual ~ASTNode() = default;
  children_t* operator->() {
    return &__children;
  }
  const children_t* operator->() const {
    return &__children;
  }
  children_t& operator*() {
    return __children;
  }
  const children_t& operator*() const {
    return __children;
  }
  children_t& children() {
    return __children;
  }
  const children_t& children() const {
    return __children;
  }
  ASTNode*& parent() {
    return __parent;
  }
  ASTNode* parent() const {
    return __parent;
  }
  auto getBeginLoc() const {
    return __range.begin;
  }
  auto getEndLoc() const {
    return __range.end;
  }
  auto getSourceRange() const {
    return __range;
  }
  template <typename T>
  T* getAsPtr() {
    return reinterpret_cast<T*>(this);
  }
  template <typename T>
  const T* getAsPtr() const {
    return reinterpret_cast<const T*>(this);
  }
  template <typename T>
  T& getAs() {
    return *reinterpret_cast<T*>(this);
  }
  template <typename T>
  const T& getAs() const {
    return *reinterpret_cast<const T*>(this);
  }
  inline bool isAttr() const {
    return _astnp_::isAttr(classOf());
  }
  inline bool isType() const {
    return _astnp_::isType(classOf());
  }
  inline bool isDecl() const {
    return _astnp_::isDecl(classOf());
  }
  inline bool isStmt() const {
    return _astnp_::isStmt(classOf());
  }
  virtual std::string to_string() const {
    return _astnp_::to_string(classOf()) + __range.to_string();
  }
protected:
  SourceRange __range { };
  ASTNode *__parent { };
  children_container<> __children;
};
struct ASTNodeList: public ASTNode, std::vector<std::unique_ptr<ASTNode>> {
  using vector_t = std::vector<std::unique_ptr<ASTNode>>;
  static constexpr node_kind_t kind = node_kind_t::ASTNodeList;
  virtual node_kind_t classOf() const {
    return kind;
  }
  using std::vector<std::unique_ptr<ASTNode>>::vector;
  using vector_t::operator=;
};
template <typename ...T>
class children_container {
public:
  static constexpr bool is_trivial = false;
  static constexpr size_t size = sizeof...(T);
  using children_nodes_t = std::tuple<T...>;
  static constexpr children_kind children_kinds[size] = { T::kind... };
  using children_types_t = std::tuple<typename T::type...>;
  static constexpr int children_offsets[size] = { T::offset... };
  static constexpr bool children_visits[size] = { T::visit... };
  template <int offset>
  using children_t = std::tuple_element_t<offset, children_types_t>;
  auto begin() {
    return data.begin();
  }
  auto begin() const {
    return data.begin();
  }
  auto end() {
    return data.end();
  }
  auto end() const {
    return data.end();
  }
  template <int offset, typename V>
  V* getAs() const {
    return reinterpret_cast<V*>(data[offset].get());
  }
  template <int offset, std::enable_if_t<children_kinds[offset] != children_kind::dynamic_list, int> = 0>
  children_t<offset>* get() const {
    return reinterpret_cast<children_t<offset>*>(data[offset].get());
  }
  template <int offset, std::enable_if_t<children_kinds[offset] != children_kind::dynamic_list, int> = 0>
  children_t<offset>& getRef() {
    return *reinterpret_cast<children_t<offset>*>(data[offset].get());
  }
  template <int offset, std::enable_if_t<children_kinds[offset] != children_kind::dynamic_list, int> = 0>
  const children_t<offset>& getRef() const {
    return *reinterpret_cast<children_t<offset>*>(data[offset].get());
  }
  template <int offset, std::enable_if_t<children_kinds[offset] == children_kind::dynamic_list, int> = 0>
  children_t<offset>* get(size_t i) const {
    return data[offset].get() ? reinterpret_cast<children_t<offset>*>(getAs<offset, ASTNodeList>()->at(i).get()) : nullptr;
  }
  template <int offset, std::enable_if_t<children_kinds[offset] == children_kind::dynamic_list, int> = 0>
  children_t<offset>& getRef(size_t i) {
    return *get<offset>(i);
  }
  template <int offset, std::enable_if_t<children_kinds[offset] == children_kind::dynamic_list, int> = 0>
  const children_t<offset>& getRef(size_t i) const {
    return *get<offset>(i);
  }
  template <int offset, std::enable_if_t<children_kinds[offset] == children_kind::dynamic_list, int> = 0>
  size_t getSize() const {
    return data[offset].get() ? getAs<offset, ASTNodeList>()->size() : 0;
  }
  template <int offset, typename V, std::enable_if_t<children_kinds[offset] == children_kind::dynamic_list, int> = 0>
  void push(V &&value) {
    if (!data[offset].get())
      create<offset>( { std::forward<V>(value) });
    else
      getAs<offset, ASTNodeList>()->push_back();
  }
  template <int offset>
  std::unique_ptr<ASTNode>& getRaw() {
    return data[offset];
  }
  template <int offset>
  const std::unique_ptr<ASTNode>& getRaw() const {
    return data[offset];
  }
  template <int offset>
  bool has() const {
    return data[offset].get();
  }
  template <int offset, typename V>
  void set(V &&value) {
    data[offset] = std::forward<V>(value);
  }
  template <int offset, typename ...Args>
  void create(Args &&... args) {
    if constexpr (children_kinds[offset] != children_kind::dynamic_list)
      data[offset] = std::make_unique<children_t<offset>>(std::forward<Args>(args)...);
    else
      data[offset] = std::make_unique<ASTNodeList>(std::forward<Args>(args)...);
  }
private:
  std::array<std::unique_ptr<ASTNode>, size> data;
};
inline std::ostream& operator<<(std::ostream &ost, const ASTNode &node) {
  ost << node.to_string();
  return ost;
}
template <typename T>
struct Reference {

};
}
#endif
