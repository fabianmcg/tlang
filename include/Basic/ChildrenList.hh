#ifndef BASIC_CHILDRENLIST_HH
#define BASIC_CHILDRENLIST_HH

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <ADT/List.hh>

namespace tlang {
enum class ChildKind {
  Static,
  Dynamic,
  StaticList,
  DynamicList
};

namespace impl {
template <typename T, ChildKind K>
struct ChildType {
  using container = T;
  using value = T;
};
template <typename T>
struct ChildType<T, ChildKind::Dynamic> {
  using container = T*;
  using value = T*;
};
template <typename T>
struct ChildType<T, ChildKind::StaticList> {
  using container = List<T>;
  using value = T;
};
template <typename T>
struct ChildType<T, ChildKind::DynamicList> {
  using container = List<T*>;
  using value = T*;
};
}

template <ChildKind Kind, typename T, int Offset, bool Visit = true>
struct Child {
  static constexpr ChildKind kind = Kind;
  using type = T;
  using container = typename impl::ChildType<type, kind>::container;
  using container_reference = container&;
  using value = typename impl::ChildType<type, kind>::value;
  using value_reference = value&;
  static constexpr int offset = Offset;
  static constexpr bool visit = Visit;
  inline static constexpr bool is(ChildKind k) {
    return k == kind;
  }
  inline static constexpr bool isDynamic() {
    return is(ChildKind::Dynamic);
  }
  inline static constexpr bool isStatic() {
    return is(ChildKind::Static);
  }
  inline static constexpr bool isDynamicList() {
    return is(ChildKind::DynamicList);
  }
  inline static constexpr bool isStaticList() {
    return is(ChildKind::StaticList);
  }
  inline static constexpr bool isList() {
    return is(ChildKind::StaticList) || is(ChildKind::DynamicList);
  }
};

template <typename ...T>
class ChildrenList;

template <>
class ChildrenList<> {
public:
  static constexpr size_t size = 0;
  using children_list = std::tuple<>;
};

template <typename ...T>
class ChildrenList {
public:
  using children_list = std::tuple<T...>;
  static constexpr size_t size = sizeof...(T);
  using children_container = std::tuple<typename T::container...>;
  static constexpr ChildKind kinds[size] = { T::kind... };
  static constexpr int offsets[size] = { T::offset... };
  static constexpr bool visits[size] = { T::visit... };

  template <int offset>
  using children_t = std::tuple_element_t<offset, children_list>;
  template <int offset>
  using container_t = std::tuple_element_t<offset, children_container>;
  template <int offset>
  using value_t = typename children_t<offset>::value;
  template <int offset>
  using container_reference_t = typename children_t<offset>::container_reference;
  template <int offset>
  using value_reference_t = typename children_t<offset>::value_reference;

  ChildrenList() = default;
  ChildrenList(ChildrenList&&) = default;
  ChildrenList(const ChildrenList&) = default;
  ChildrenList& operator=(ChildrenList&&) = default;
  ChildrenList& operator=(const ChildrenList&) = default;

  template <typename ...Args>
  ChildrenList(Args &&...args) :
      data(std::forward<Args>(args)...) {
  }
  template <typename ...Args>
  ChildrenList(const Args &...args) :
      data(args...) {
  }

  inline children_container& operator*() {
    return data;
  }
  inline const children_container& operator*() const {
    return data;
  }
  template <int offset>
  inline container_reference_t<offset> get() {
    return std::get<offset>(data);
  }
  template <int offset>
  inline container_t<offset> const& get() const {
    return std::get<offset>(data);
  }
  template <int offset, std::enable_if_t<children_t<offset>::isList(), int> = 0>
  inline value_reference_t<offset> getElem(size_t i) {
    return get<offset>()[i];
  }
  template <int offset, std::enable_if_t<children_t<offset>::isList(), int> = 0>
  inline const value_reference_t<offset> getElem(size_t i) const {
    return get<offset>()[i];
  }
  template <int offset, std::enable_if_t<children_t<offset>::isList(), int> = 0>
  inline size_t getSize() const {
    return get<offset>().size();
  }
  template <int offset>
  inline bool has() const {
    if constexpr (children_t<offset>::isDynamic())
      return get<offset>();
    return true;
  }
private:
  children_container data { };
};
}
#endif
