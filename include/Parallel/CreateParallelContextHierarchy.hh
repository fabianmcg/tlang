#ifndef PARALLEL_CREATEPARALLELCONTEXTHIERARCHY_HH
#define PARALLEL_CREATEPARALLELCONTEXTHIERARCHY_HH

#include <deque>
#include <AST/Visitors/EditVisitor.hh>
#include <Parallel/Pipeline.hh>
#include <Support/Format.hh>

namespace tlang {
struct CreateParallelContextHierarchy: public CompilerInvocationRef, PassBase<CreateParallelContextHierarchy> {
  using CompilerInvocationRef::CompilerInvocationRef;
  struct Visitor: public CompilerInvocationRef, public EditVisitor<Visitor, VisitorPattern::prePostOrder> {
    using CompilerInvocationRef::CompilerInvocationRef;
    visit_t visitParallelConstruct(ParallelConstruct *node, AnyASTNodeRef ref, VisitType kind) {
      auto parentContext = constructStack.size() ? constructStack.front() : nullptr;
      if (kind == preVisit)
        node->getContext().data() = parentContext;
      return visit;
    }
    visit_t visitContextStmt(ContextStmt *context, AnyASTNodeRef ref, VisitType kind) {
      if (kind == preVisit)
        resolveContextStmt(context);
      manageContextStack(context, kind);
      managConstructStack(context, kind);
      return visit;
    }
    visit_t visitParallelStmt(ParallelStmt *stmt, AnyASTNodeRef ref, VisitType kind) {
      managParallelStack(stmt, kind);
      managConstructStack(stmt, kind);
      return visit;
    }
    visit_t visitLoopStmt(LoopStmt *loop, AnyASTNodeRef ref, VisitType kind) {
      managParallelStack(loop, kind);
      managConstructStack(loop, kind);
      auto parentContext = parallelStack.size() ? parallelStack.front() : nullptr;
      if (kind == postVisit && parentContext == nullptr) {
        auto ps = CI.getContext().make<ParallelStmt>(nullptr, List<TupleExpr*>(), loop);
        loop->getContext() = ps;
        auto cs = CI.getContext().make<ContextStmt>(ContextStmt::Inherited, List<MapStmt*>(), ps);
        ps->getContext() = cs;
        ref.assign<Stmt>(cs);
        resolveContextStmt(cs);
        cs->getContext() = constructStack.size() ? constructStack.front() : nullptr;
      }
      return visit;
    }
    void resolveContextStmt(ContextStmt *context) {
      auto parentContext = contextStack.size() ? contextStack.front() : nullptr;
      if (contextStack.empty() && (context->isDefault() || context->isInherited()))
        context->getContextKind() = getDefaultContextKind();
      else if (parentContext != nullptr && (context->isDefault() || context->isInherited()))
        context->getContextKind() = context->isInherited() ? parentContext->getContextKind() : getDefaultContextKind();
    }
    ContextStmt::Kind getDefaultContextKind() {
      auto ck = CI.getOptions().langOpts.defaultContext;
      if (ck == ParallelLangOpts::Device)
        return ContextStmt::Device;
      return ContextStmt::Host;
    }
    visit_t manageContextStack(ContextStmt *node, VisitType kind) {
      if (kind == preVisit)
        contextStack.push_front(node);
      else if (contextStack.size())
        contextStack.pop_front();
      return visit;
    }
    visit_t managParallelStack(ParallelConstruct *node, VisitType kind) {
      if (kind == preVisit)
        parallelStack.push_front(node);
      else if (parallelStack.size())
        parallelStack.pop_front();
      return visit;
    }
    visit_t managConstructStack(ParallelConstruct *node, VisitType kind) {
      if (kind == preVisit)
        constructStack.push_front(node);
      else if (constructStack.size())
        constructStack.pop_front();
      return visit;
    }
    std::deque<ContextStmt*> contextStack;
    std::deque<ParallelConstruct*> parallelStack;
    std::deque<ParallelConstruct*> constructStack;
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
