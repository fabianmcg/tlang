#ifndef __ANALYSIS_ANALYSIS_CONTEXT_HH__
#define __ANALYSIS_ANALYSIS_CONTEXT_HH__

#include <AST/ASTNode.hh>
#include "AnalysisInfo.hh"

namespace tlang {
class AnalysisContext {
public:
  AnalysisContext() = default;
  ~AnalysisContext() = default;
  AnalysisContext(AnalysisContext&&) = default;
  AnalysisContext(const AnalysisContext&) = delete;
  AnalysisContext& operator=(AnalysisContext&&) = default;
  AnalysisContext& operator=(const AnalysisContext&) = delete;
  AnalysisInfo* operator[](ASTNode *key) const {
    auto it = context.find(key);
    if (it != context.end())
      return it->second.get();
    return nullptr;
  }
  template <typename T, typename ...Args>
  static inline std::unique_ptr<AnalysisInfo> makeInfo(Args &&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }
  template <typename T>
  static inline std::unique_ptr<AnalysisInfo> createInfo(T &&value) {
    return std::make_unique<T>(std::forward<T>(value));
  }
  template <typename T, typename ...Args>
  inline T* make(ASTNode *key, Args &&... args) {
    auto &result = context[key] = makeInfo<T>(std::forward<Args>(args)...);
    return static_cast<T*>(result.get());
  }
  template <typename T>
  inline T* create(ASTNode *key, T &&value) {
    auto &result = context[key] = createInfo<T>(std::forward<T>(value));
    return static_cast<T*>(result.get());
  }
  inline std::unique_ptr<AnalysisInfo> exchange(ASTNode *key, std::unique_ptr<AnalysisInfo> &&result) {
    auto tmp = std::move(context[key]);
    context[key] = std::move(result);
    return tmp;
  }
private:
  std::map<ASTNode*, std::unique_ptr<AnalysisInfo>> context;
};
}

#endif
