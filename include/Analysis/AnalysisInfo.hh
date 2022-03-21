#ifndef __ANALYSIS_ANALYSIS_RESULTS_HH__
#define __ANALYSIS_ANALYSIS_RESULTS_HH__

#include <AST/ASTNode.hh>

namespace tlang {
class AnalysisInfo {
public:
  AnalysisInfo() = default;
  virtual ~AnalysisInfo() = default;
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
};
}

#endif
