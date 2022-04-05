#ifndef AST_VISITORS_EDITHELPER_HH
#define AST_VISITORS_EDITHELPER_HH

#include <AST/ASTNode.hh>
#include <AST/Attr.hh>
#include <AST/Decl.hh>
#include <AST/Expr.hh>
#include <AST/Stmt.hh>
#include <AST/Type.hh>
#include <AST/DeclContext.hh>
#include <AST/Traits.hh>
#include <Support/StaticFor.hh>
#include <Support/ReverseRange.hh>

namespace tlang {
enum class EditVisitorInfoKind {
  Dynamic,
  Static,
  DynamicList,
  StaticList,
  DeclContext
};

template <EditVisitorInfoKind Kind, typename Child>
struct EditVisitorInfo;
template <typename Child>
struct EditVisitorInfo<EditVisitorInfoKind::Dynamic, Child> {
  static constexpr EditVisitorInfoKind kind = EditVisitorInfoKind::Dynamic;
  Child *&child { };
};
template <typename Child>
struct EditVisitorInfo<EditVisitorInfoKind::Static, Child> {
  static constexpr EditVisitorInfoKind kind = EditVisitorInfoKind::Static;
  Child &child { };
};
template <typename Child>
struct EditVisitorInfo<EditVisitorInfoKind::DynamicList, Child> {
  static constexpr EditVisitorInfoKind kind = EditVisitorInfoKind::DynamicList;
  Child *&child { };
  bool deleteChild = false;
  inline void setDelete() {
    deleteChild = true;
  }
};
template <typename Child>
struct EditVisitorInfo<EditVisitorInfoKind::StaticList, Child> {
  static constexpr EditVisitorInfoKind kind = EditVisitorInfoKind::StaticList;
  Child &child { };
  bool deleteChild = false;
  inline void setDelete() {
    deleteChild = true;
  }
};
template <typename Child>
struct EditVisitorInfo<EditVisitorInfoKind::DeclContext, Child> {
  static constexpr EditVisitorInfoKind kind = EditVisitorInfoKind::DeclContext;
  Child &child { };
  bool deleteChild = false;
  inline void setDelete() {
    deleteChild = true;
  }
};

namespace tlang::impl {
struct EditAddChildren {
  template <typename NodeInfo>
  struct TupleVisitor {
    NodeInfo &node_info;
    template <int it, typename NodeContainer>
    void execute(NodeContainer &value) {
      using node_type = typename NodeInfo::node_type;
      using child_info = typename node_type::children_list::children_t<it>;
      using child_type = typename node_type::children_list::value_reference_t<it>;
      if constexpr (child_info::isList()) {
        if constexpr (child_info::isDynamicList()) {
          auto it = value.begin();
          while (it != value.end()) {
            EditVisitorInfo<EditVisitorInfoKind::DynamicList, child_type> data { *it };
            node_info(data, *it);
            if (data.deleteChild) {
              it = value.erase(it);
              continue;
            }
            ++it;
          }
        } else {
          auto it = value.begin();
          while (it != value.end()) {
            EditVisitorInfo<EditVisitorInfoKind::StaticList, child_type> data { *it };
            node_info(data, *it);
            if (data.deleteChild) {
              it = value.erase(it);
              continue;
            }
            ++it;
          }
        }
      } else {
        if constexpr (child_info::isDynamic()) {
          EditVisitorInfo<EditVisitorInfoKind::Dynamic, child_type> data { value };
          node_info(data, value);
        } else {
          EditVisitorInfo<EditVisitorInfoKind::Static, child_type> data { value };
          node_info(data, &value);
        }
      }
    }
  };
  template <typename NodeInfo>
  static inline void EditAddChildren(NodeInfo &&sn) {
    TupleVisitor<NodeInfo> visitor { sn };
    visit_tuple((****sn), visitor);
  }
  template <typename NodeInfo>
  struct NodeVisitor {
    using type = typename NodeInfo::node_type;
    using parent_list = typename type::parent_list;
    NodeInfo &node_info;
    template <typename T>
    static inline NodeVisitor<T> make(T &&value) {
      return NodeVisitor<T> { value };
    }
    template <typename T, std::enable_if_t<IsDeclContext<T>::value, int> = 0>
    inline void visit(T *context) {
      if (context->visit())
        for (auto &decl : make_reverse(*context))
          node_info.push(*decl);
    }
    template <typename T, std::enable_if_t<isValidNode_v<T>, int> = 0>
    inline void visit(T *node) {
      if constexpr (T::hasChildren())
        EditAddChildren::EditAddChildren(NodeInfo::make(node_info, node));
    }
    template <typename T, std::enable_if_t<!(isValidNode_v<T> || IsDeclContext<T>::value), int> = 0>
    inline void visit(T*) {
    }
    template <int it>
    inline void execute() {
      if constexpr (parent_list::size) {
        using parent_type = typename parent_list::parent_t<it>;
        auto asParent = static_cast<parent_type*>(*node_info);
        visit(asParent);
        if constexpr (isValidNode_v<parent_type>)
          make(NodeInfo::make(node_info, asParent)).init();
      }
    }
    inline void init() {
      static_for<0, parent_list::size>::execute(*this);
    }
  };
  template <typename NodeInfo>
  void operator()(NodeInfo &&nodeStack) {
    using type = typename NodeInfo::node_type;
    if constexpr (type::hasChildren())
      EditAddChildren::EditAddChildren(std::forward<NodeInfo>(nodeStack));
    NodeVisitor<NodeInfo> { nodeStack }.init();
  }
};
}
}
#endif
