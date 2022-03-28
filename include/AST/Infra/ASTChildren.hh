#ifndef __CHIL__
#define __CHIL__

#include <cstdint>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>
#include <Common/StaticFor.hh>
#include <Common/Reference.hh>

namespace tlang {
template <typename T>
using List = std::vector<T>;
using Identifier = std::string;
enum class ChildKind {
  Static,
  Dynamic,
  StaticList,
  DynamicList
};
template <ChildKind K>
constexpr bool is_list() {
  return K == ChildKind::StaticList || K == ChildKind::DynamicList;
}
template <ChildKind K>
constexpr bool is_dynamic() {
  return K == ChildKind::Dynamic || K == ChildKind::DynamicList;
}
template <typename T, ChildKind K>
struct child_container_type {
  using type = T;
  using value_type = T;
  using return_type = type&;
};
template <typename T>
struct child_container_type<T, ChildKind::Dynamic> {
  using type = T*;
  using value_type = T*;
  using return_type = type&;
};
template <typename T>
struct child_container_type<T, ChildKind::StaticList> {
  using type = std::vector<T>;
  using value_type = T;
  using return_type = type&;
};
template <typename T>
struct child_container_type<T, ChildKind::DynamicList> {
  using type = std::vector<T*>;
  using value_type = T*;
  using return_type = type&;
};
template <typename T, ChildKind K>
using child_container_t = child_container_type<T, K>;

template <ChildKind K, typename T, int O, bool V = true>
struct Child {
  static constexpr ChildKind kind = K;
  using type = T;
  using value_type = typename child_container_t<type, K>::value_type;
  using return_type = typename child_container_t<type, K>::return_type;
  using container_type = typename child_container_t<type, K>::type;
  static constexpr int offset = O;
  static constexpr bool visit = V;
  static constexpr bool dynamic = is_dynamic<kind>();
  static constexpr bool list = is_list<kind>();
};

template <typename ASTNode, typename ...T>
class children_container_helper;
template <typename ASTNode>
class children_container_helper<ASTNode> {
public:
  static constexpr bool is_trivial = true;
  static constexpr size_t size = 0;
  template <int offset>
  using return_t = ASTNode*;
  template <int offset>
  using value_t = ASTNode*;
  template <typename ...Args>
  children_container_helper(Args &&...args) {
  }
  template <bool reversed = false, typename T = int>
  void traverse(T &&function) {
  }
  template <bool reversed = false, typename T = int>
  void traverse(T &&function) const {
  }
  template <int offset>
  inline return_t<offset> get() const {
    return nullptr;
  }
  template <int offset>
  inline value_t<offset> getElem(size_t i) const {
    return nullptr;
  }
};
template <typename ASTNode, typename ...T>
class children_container_helper {
public:
  static constexpr bool is_trivial = false;
  static constexpr size_t size = sizeof...(T);
  using children_nodes_t = std::tuple<T...>;
  static constexpr ChildKind children_kinds[size] = { T::kind... };
  using children_types = std::tuple<typename T::type...>;
  using children_value_types = std::tuple<typename T::value_type...>;
  using children_return_types = std::tuple<typename T::return_type...>;
  using children_container_types = std::tuple<typename T::container_type...>;
  static constexpr int children_offsets[size] = { T::offset... };
  static constexpr bool children_visits[size] = { T::visit... };
  template <int offset>
  using children_t = std::tuple_element_t<offset, children_types>;
  template <int offset>
  using container_t = std::tuple_element_t<offset, children_container_types>;
  template <int offset>
  using return_t = std::tuple_element_t<offset, children_return_types>;
  template <int offset>
  using value_t = std::tuple_element_t<offset, children_value_types>&;
  children_container_helper() = default;
//  template <typename V>
//  static const V& fwd(const V &value) {
//    return value;
//  }
//  template <typename VV, typename V = VV, std::enable_if_t<std::is_rvalue_reference_v<V>, int> = 0>
//  static std::decay_t<VV>&& fwd(VV &&value) {
//    return std::forward<V>(value);
//  }
//  template <typename V, std::enable_if_t<!std::is_rvalue_reference_v<V>, int> = 0>
//  static std::decay_t<V> fwd(V &&value) {
//    return value;
//  }
//  template <typename ...Args>
//  children_container_helper(Args &&...args) :
//      __data(fwd<Args>(args)...) {
//  }
  template <typename ...Args>
  children_container_helper(const Args &...args) :
      __data(args...) {
  }
  children_container_helper(children_container_helper&&) = default;
  children_container_helper(const children_container_helper&) = default;
  children_container_helper& operator=(children_container_helper&&) = default;
  children_container_helper& operator=(const children_container_helper&) = default;
  template <int offset, typename V = container_t<offset>, std::enable_if_t<!is_list<children_kinds[offset]>(), int> = 0>
  V* getAs() const {
    if constexpr (children_kinds[offset] == ChildKind::Dynamic)
      return dynamic_cast<V*>(data<offset>());
    else
      return dynamic_cast<V*>(const_cast<container_t<offset>*>(&data<offset>()));
  }
  template <int offset>
  inline return_t<offset> get() {
    return data<offset>();
  }
  template <int offset>
  inline const return_t<offset> get() const {
    return data<offset>();
  }
  template <int offset, std::enable_if_t<is_list<children_kinds[offset]>(), int> = 0>
  inline value_t<offset> getElem(size_t i) {
    return get<offset>()[i];
  }
  template <int offset, std::enable_if_t<is_list<children_kinds[offset]>(), int> = 0>
  inline const value_t<offset> getElem(size_t i) const {
    return get<offset>()[i];
  }
  template <int offset, std::enable_if_t<!is_list<children_kinds[offset]>(), int> = 0>
  inline size_t getSize() const {
    return 1;
  }
  template <int offset, std::enable_if_t<is_list<children_kinds[offset]>(), int> = 0>
  inline size_t getSize() const {
    return data<offset>().size();
  }
  template <int offset>
  inline bool has() const {
    if constexpr (children_kinds[offset] == ChildKind::Dynamic)
      return data<offset>();
    return true;
  }
  template <bool reversed = false, typename V = int>
  void traverse(V &&function) {
    visit_tuple<reversed>(__data, std::forward<V>(function));
  }
  template <bool reversed = false, typename V = int>
  void traverse(V &&function) const {
    visit_tuple<reversed>(__data, std::forward<V>(function));
  }
private:
  children_container_types __data { };
  template <int offset>
  return_t<offset> data() {
    return std::get<offset>(__data);
  }
  template <int offset>
  const return_t<offset> data() const {
    return const_cast<const return_t<offset>>(std::get<offset>(__data));
  }
};
}
#endif
