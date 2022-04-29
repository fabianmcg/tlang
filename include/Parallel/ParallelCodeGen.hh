//#ifndef PASSES_PARALLELPIPELINE_PARALLELCODEGEN_HH
//#define PASSES_PARALLELPIPELINE_PARALLELCODEGEN_HH
//
//#include <Sema/Sema.hh>
//#include "Analysis.hh"
//
//namespace tlang {
//struct ParallelCodeGen: public ASTContextReference, public PassBase<ParallelCodeGen> {
//  struct ChangeAS: ASTVisitor<ChangeAS, VisitorPattern::prePostOrder> {
//    visit_t visitDeclRefExpr(DeclRefExpr *expr, VisitType kind) {
//      if (kind == postVisit)
//        return visit;
//      if (auto vd = dyn_cast<VariableDecl>(expr->getDecl().data())) {
//        if (vd->getType().getAddressSpace())
//          expr->getType().getAddressSpace() = vd->getType().getAddressSpace();
//      }
//      return visit;
//    }
//    visit_t visitArrayExpr(ArrayExpr *expr, VisitType kind) {
//      if (kind == postVisit)
//        if (int as = expr->getArray()->getType().getAddressSpace())
//          expr->getType().getAddressSpace() = as;
//      return visit;
//    }
//    visit_t visitBinaryOperator(BinaryOperator *expr, VisitType kind) {
//      if (kind == postVisit)
//        if (expr->getOp() == BinaryOperator::Assign)
//          if (int as = expr->getLhs()->getType().getAddressSpace())
//            expr->getType().getAddressSpace() = as;
//      return visit;
//    }
//    visit_t visitUnaryOperator(UnaryOperator *expr, VisitType kind) {
//      if (kind == postVisit)
//        if (int as = expr->getExpr()->getType().getAddressSpace())
//          expr->getType().getAddressSpace() = as;
//      return visit;
//    }
//  };
//  using ASTContextReference::ASTContextReference;
//  bool run(ModuleDecl &module, AnyASTNodeRef nodeRef, ResultManager &manager) {
//    auto unit = context.addUnit("nvptx", UnitDecl::NVPTX);
//    auto pmodule = context.make<ModuleDecl>(NamedDecl("NVPTX"), DeclContext());
//    context.addModule(unit, pmodule);
//    ParallelAnalysis *analysis = manager.getResult<ParallelAnalysis>(ParallelAnalysisPass::ID(), &module);
//    if (analysis)
//      for (auto &a : analysis->parallelSections) {
//        eraseSection(a.ref);
//        genFunction(pmodule, a.section);
//      }
//    return true;
//  }
//  void eraseSection(AnyASTNodeRef &ref) {
//    ref.assign<Stmt, Stmt>(nullptr);
//  }
//  void genFunction(ModuleDecl *module, Stmt *stmt) {
////    if (auto section = dyn_cast<BlockStmt>(stmt)) {
////      List<ParameterDecl*> parameters;
////      for (auto expr : section->getPrivateVariables()) {
////        if (auto ref = dyn_cast<DeclRefExpr>(expr)) {
////          if (auto vd = dyn_cast<VariableDecl>(ref->getDecl().data())) {
////            auto type = vd->getType();
////            if (isa<PtrType>(type.getType()))
////              type.getAddressSpace() = 1;
////            parameters.push_back(builder.CreateParameter(vd->getIdentifier(), type));
////          }
////        }
////      }
////      auto function = builder.AddToContext(module,
////          builder.CreateFunction("nvptx_kernel", QualType(), move(parameters), dyn_cast<CompoundStmt>(section->getStmt())));
////      Sema sema(context);
////      sema.resolveNames(function);
////      function->getFunctionKind() = FunctorDecl::Kernel;
////      ChangeAS { }.traverseFunctionDecl(function);
////    }
//  }
//  ASTApi builder { context };
//};
//}
//
//#endif
