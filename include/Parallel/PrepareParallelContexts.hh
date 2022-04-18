#ifndef PARALLEL_PREPARAPARALLELCONTEXTS_HH
#define PARALLEL_PREPARAPARALLELCONTEXTS_HH

#include <deque>
#include <AST/Visitors/EditVisitor.hh>
#include <Parallel/Pipeline.hh>
#include <Support/Format.hh>

namespace tlang {
struct PrepareParallelContexts: public CompilerInvocationRef, PassBase<PrepareParallelContexts> {
  using CompilerInvocationRef::CompilerInvocationRef;
  struct Visitor: public CompilerInvocationRef, public EditVisitor<Visitor, VisitorPattern::prePostOrder> {
    using CompilerInvocationRef::CompilerInvocationRef;
    visit_t visitContextStmt(ContextStmt *context, AnyASTNodeRef ref, VisitType kind) {
      auto parentContext = contextStack.size() ? contextStack.front() : nullptr;
      if (kind == preVisit) {
        ContextStmt *pc = dyn_cast<ContextStmt>(parentContext);
        if (parentContext && !pc)
          throw(std::runtime_error("A ContextStmt cannot be inside a non ContextStmt stmt"));
        context->getParentContext().data() = parentContext;
        if (parentContext == nullptr && (context->isDefault() || context->isInherited())) {
          context->getContextKind() = getDefaultContextKind();
        } else if (parentContext != nullptr && (context->isDefault() || context->isInherited())) {
          auto pk = getParentContextStmtKind(parentContext);
          auto ck = getDefaultContextKind();
          context->getContextKind() = context->isInherited() ? pc->getContextKind() : ck;
        }
      }
      manageContext(context, kind);
      return visit;
    }
    visit_t visitParallelContext(ParallelContext *node, AnyASTNodeRef ref, VisitType kind) {
      if (node->getParentContext().data())
        return manageContext(node, kind);
      auto parentContext = contextStack.size() ? contextStack.front() : nullptr;
      if (kind == preVisit) {
        if (parentContext == nullptr) {
          auto context = CI.getContext().make<ContextStmt>(nullptr, getDefaultContextKind(), List<MapStmt*> { }, node);
          ref.assign<Stmt>(context);
          node->getParentContext().data() = context;
          AnyASTNodeRef ref { context->getStmt() };
          dynamicTraverse(node, ref);
          return skip;
        }
        if (parentContext != node)
          node->getParentContext().data() = parentContext;
      }
      manageContext(node, kind);
      return visit;
    }
    visit_t visitParallelIntrinsic(ParallelIntrinsic *node, AnyASTNodeRef ref, VisitType kind) {
      auto parentContext = contextStack.size() ? contextStack.front() : nullptr;
      if (kind == preVisit)
        node->getParentContext().data() = parentContext;
      return visit;
    }
    ContextStmt::Kind getDefaultContextKind() {
      auto ck = CI.getOptions().langOpts.defaultContext;
      if (ck == ParallelLangOpts::Device)
        return ContextStmt::Device;
      return ContextStmt::Host;
    }
    ContextStmt::Kind getParentContextStmtKind(ParallelBaseContext *parentContext) {
      ContextStmt *pc = dyn_cast<ContextStmt>(parentContext);
      if (pc)
        return pc->getContextKind();
      return getDefaultContextKind();
    }
    visit_t manageContext(ParallelBaseContext *node, VisitType kind) {
      if (kind == preVisit)
        contextStack.push_front(node);
      else if (contextStack.size())
        contextStack.pop_front();
      return visit;
    }
    std::deque<ParallelBaseContext*> contextStack;
  };
  bool run(UnitDecl &universe, AnyASTNodeRef ref, ResultManager &results) {
    print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::yellow_green), "Resolving parallel contexts\n");
    Visitor { this->CI }.traverseUnitDecl(&universe, ref);
    print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::lime_green), "Finished resolving parallel contexts\n");
    return true;
  }
};
}
#endif
