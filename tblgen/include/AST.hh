#ifndef __AST_HH__
#define __AST_HH__

#include <list>
#include <deque>
#include <ASTNode.hh>

template <typename V>
class TreeNode {
public:
  using value_type = V;
  using list = std::list<TreeNode*>;
  TreeNode(llvm::Record &record, TreeNode *parentNode = nullptr) :
      parentNode(parentNode), node { record } {
  }
  value_type& operator*() {
    return node;
  }
  const value_type& operator*() const {
    return node;
  }
  value_type* operator->() {
    return &node;
  }
  const value_type* operator->() const {
    return &node;
  }
  list& children() {
    return childrenList;
  }
  const list& children() const {
    return childrenList;
  }
  void add(TreeNode *node) {
    if (node)
      childrenList.push_back(node);
  }
  int& visits() {
    return visitCount;
  }
  int visits() const {
    return visitCount;
  }
  TreeNode*& parent() {
    return parentNode;
  }
  const TreeNode* parent() const {
    return parentNode;
  }
private:
  TreeNode *parentNode { };
  list childrenList { };
  value_type node;
  int visitCount { };
};
template <typename V>
class AST {
public:
  using value_type = V;
  using tree_node = TreeNode<value_type>;
  AST(llvm::RecordKeeper &records) {
    init(records);
  }
  template <typename F>
  void dfs(F &&function) {
    if (root) {
      std::deque<tree_node*> stack { root };
      while (stack.size()) {
        auto node = stack.front();
        stack.pop_front();
        if (!node)
          continue;
        function(node);
        auto &children = node->children();
        for (auto it = children.rbegin(); it != children.rend(); ++it)
          stack.push_front(*it);
      }
    }
  }
  template <typename F>
  static void bfs(F &&function, tree_node *root) {
    if (root) {
      std::deque<tree_node*> stack { root };
      while (stack.size()) {
        auto node = stack.front();
        stack.pop_front();
        if (!node)
          continue;
        function(node);
        for (auto n : node->children())
          stack.push_back(n);
      }
    }
  }
  template <typename F>
  void bfs(F &&function) {
    bfs(std::forward<F>(function), root);
  }
  template <typename PRE, typename POST>
  void bfsRecursive(tree_node *node, PRE &&pre, POST &&post) {
    if (node) {
      pre(*node);
      for (auto n : node->children())
        bfsRecursive(n, std::forward<PRE>(pre), std::forward<POST>(post));
      post(*node);
    }
  }
  template <typename PRE, typename POST>
  void bfsRecursive(PRE &&pre, POST &&post) {
    bfsRecursive(root, std::forward<PRE>(pre), std::forward<POST>(post));
  }
private:
  tree_node& addNode(llvm::Record &record) {
    auto pi = tree.insert( { &record, tree_node { record } });
    return pi.first->second;
  }
  void add(llvm::Record *record) {
    if (record) {
      auto &node = addNode(*record);
      if ((*node).hasBase()) {
        auto parentRecord = &(*(*node).base());
        auto &parent = addNode(*parentRecord);
        parent.add(&node);
        node.parent() = &parent;
      } else
        root = &node;
    }
  }
  void init(llvm::RecordKeeper &records) {
    auto nodes = records.getAllDerivedDefinitions(value_type::kind);
    for (auto node : nodes)
      add(node);
  }
private:
  std::map<llvm::Record*, tree_node> tree { };
  tree_node *root { };
};

#endif
