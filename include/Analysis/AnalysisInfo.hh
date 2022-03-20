#ifndef __ANALYSIS_ANALYSIS_RESULTS_HH__
#define __ANALYSIS_ANALYSIS_RESULTS_HH__

#include <AST/ASTNode.hh>

namespace tlang {
class AnalysisInfo {
public:
  AnalysisInfo() = default;
  inline AnalysisInfo(ASTNode *node, NodeClass nodeKind) :
      node(node), nodeKind(nodeKind) {
  }
  inline AnalysisInfo(ASTNode *node) :
      node(node) {
    if (node)
      nodeKind = node->classOf();
  }
  inline ASTNode* operator*() const {
    return node;
  }
  inline NodeClass kind() const {
    return nodeKind;
  }
  inline bool is(NodeClass kind) const {
    return kind == nodeKind;
  }
  inline bool isNot(NodeClass kind) const {
    return kind != nodeKind;
  }
  template <typename T>
  inline T* getAs() {
    return static_cast<T*>(this);
  }
  template <typename T>
  inline const T* getAs() const {
    return static_cast<T*>(this);
  }
  template <typename T>
  inline T* getDynAs() {
    return dynamic_cast<T*>(this);
  }
  template <typename T>
  inline const T* getDynAs() const {
    return dynamic_cast<T*>(this);
  }
protected:
  ASTNode *node { };
  NodeClass nodeKind { NodeClass::ASTNode };
};
}

#endif
