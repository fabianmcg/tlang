#ifndef __CHIL__
#define __CHIL__

#include <cstdint>
#include <memory>
#include <tuple>
#include <type_traits>
#include "ASTCommon.hh"

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

template <typename ASTNode, access_kind kind = access_kind::pointer>
struct access_type;
template <typename ASTNode>
struct access_type<ASTNode, access_kind::pointer> {
  template <typename T>
  using type = T*;
};
template <typename ASTNode>
struct access_type<ASTNode, access_kind::reference> {
  template <typename T>
  using type = T&;
};
template <typename ASTNode>
struct access_type<ASTNode, access_kind::container> {
  template <typename T>
  using type = std::unique_ptr<ASTNode>&;
};

enum class child_kind {
  static_node,
  dynamic_node,
  static_list,
  dynamic_list
};
template <child_kind K, typename T, int O, bool V = true>
struct child_node {
  static constexpr child_kind kind = K;
  using type = T;
  static constexpr int offset = O;
  static constexpr bool visit = V;
};

template <typename ASTNode, typename ASTNodeList, typename ...T>
class children_container_helper;
template <typename ASTNode, typename ASTNodeList>
class children_container_helper<ASTNode, ASTNodeList> {
public:
  static constexpr bool is_trivial = true;
  static constexpr size_t size = 0;
  template <int offset, access_kind kind>
  using return_t = typename access_type<ASTNode, kind>::template type<ASTNode>;
  template <typename ...Args>
  children_container_helper(Args &&...args) {
  }
  ASTNode* begin() const {
    return nullptr;
  }
  ASTNode* end() const {
    return nullptr;
  }
  ASTNode* rbegin() const {
    return nullptr;
  }
  ASTNode* rend() const {
    return nullptr;
  }
};
template <typename ASTNode, typename ASTNodeList, typename ...T>
class children_container_helper {
public:
  static constexpr bool is_trivial = false;
  static constexpr size_t size = sizeof...(T);
  using children_nodes_t = std::tuple<T...>;
  static constexpr child_kind children_kinds[size] = { T::kind... };
  using children_types_t = std::tuple<typename T::type...>;
  static constexpr int children_offsets[size] = { T::offset... };
  static constexpr bool children_visits[size] = { T::visit... };
  template <int offset>
  using children_t = std::tuple_element_t<offset, children_types_t>;
  template <int offset, access_kind kind>
  using return_t = typename access_type<ASTNode, kind>::template type<children_t<offset>>;
  template <typename ...Args>
  children_container_helper(Args &&...args) :
      data(std::forward<Args>(args)...) {
  }
  auto begin() {
    return data.begin();
  }
  auto begin() const {
    return data.begin();
  }
  auto rbegin() {
    return data.rbegin();
  }
  auto rbegin() const {
    return data.rbegin();
  }
  auto end() {
    return data.end();
  }
  auto end() const {
    return data.end();
  }
  auto rend() {
    return data.rend();
  }
  auto rend() const {
    return data.rend();
  }
  template <int offset, typename V>
  V* getAs() const {
    return dynamic_cast<V*>(data[offset].get());
  }
  template <int offset, access_kind kind, std::enable_if_t<children_kinds[offset] != child_kind::dynamic_list, int> = 0>
  return_t<offset, kind> get() {
    if constexpr (kind == access_kind::pointer)
      return static_cast<children_t<offset>*>(data[offset].get());
    else if constexpr (kind == access_kind::reference)
      return *static_cast<children_t<offset>*>(data[offset].get());
    else if constexpr (kind == access_kind::container)
      return data[offset];
  }
  template <int offset, access_kind kind, std::enable_if_t<children_kinds[offset] != child_kind::dynamic_list, int> = 0>
  const return_t<offset, kind> get() const {
    if constexpr (kind == access_kind::pointer)
      return static_cast<children_t<offset>*>(data[offset].get());
    else if constexpr (kind == access_kind::reference)
      return *static_cast<children_t<offset>*>(data[offset].get());
    else if constexpr (kind == access_kind::container)
      return data[offset];
  }
//  template <int offset, std::enable_if_t<children_kinds[offset] == child_kind::dynamic_list, int> = 0>
//  size_t getSize() const {
//    return data[offset].get() ? getAs<offset, ASTNodeList>()->size() : 0;
//  }
  template <int offset>
  bool has() const {
    return data[offset].get();
  }
private:
  std::array<std::unique_ptr<ASTNode>, size> data;
};

}

#endif
