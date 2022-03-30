#ifndef AST_VISITORS_VISITORHELPER_HH
#define AST_VISITORS_VISITORHELPER_HH

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

namespace tlang::impl {
struct AddChildren {
  template <typename NodeInfo>
  struct TupleVisitor {
    NodeInfo &node_info;
    template <int it, typename NodeContainer>
    void execute(NodeContainer &&value) {
      using child_info = typename NodeInfo::node_type::children_list::children_t<it>;
      if constexpr (child_info::isList()) {
        if constexpr (child_info::isDynamicList()) {
          for (auto child : make_reverse(value))
            node_info.push(child);
        } else
          for (auto &child : make_reverse(value))
            node_info.push(&child);
      } else {
        if constexpr (child_info::isDynamic())
          node_info.push(value);
        else
          node_info.push(&value);
      }
    }
  };
  template <typename NodeInfo>
  static inline void addChildren(NodeInfo &&sn) {
    TupleVisitor<NodeInfo> visitor { sn };
    visit_tuple<true>((****sn), visitor);
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
      for (auto &decl : make_reverse(*context))
        node_info.push(*decl);
    }
    template <typename T, std::enable_if_t<isValidNode_v<T>, int> = 0>
    inline void visit(T *node) {
      if constexpr (T::hasChildren())
        AddChildren::addChildren(NodeInfo::make(node_info, node));
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
      AddChildren::addChildren(std::forward<NodeInfo>(nodeStack));
    NodeVisitor<NodeInfo> { nodeStack }.init();
  }
};
}
#endif
