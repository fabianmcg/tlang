#ifndef __ANALYSIS_ANALYSIS_HH__
#define __ANALYSIS_ANALYSIS_HH__

#include "AST/ASTContext.hh"

namespace tlang {
struct CodeAnalysis {
  CodeAnalysis(ASTContext &context) :
      context(context) {
  }
  void run();
  ASTContext &context;
};
}

#endif
