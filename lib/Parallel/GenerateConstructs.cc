#include <AST/Api.hh>
#include <AST/Visitors/ASTVisitor.hh>
#include <Parallel/GenerateConstructs.hh>

namespace tlang {
bool GenerateConstructs::run(UnitDecl &decl, AnyASTNodeRef ref, ResultManager &results) {
  unit = &decl;
  auto constructs = results.getResult<ParallelConstructDatabase>(CreateConstructDatabase::ID(), &decl);
  if (constructs) {
    print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::yellow_green), "Transforming parallel constructs\n");
    removeContexts(*constructs);
    addAPI(*constructs);
    generateParallelRegions(*constructs);
    generateContexts(*constructs);
    print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::lime_green), "Finished transforming parallel constructs\n");
  }
  return true;
}

void GenerateConstructs::removeContexts(ParallelConstructDatabase &constructs) {
//  for (auto &c : constructs.contexts)
//    c.construct.reference.makeNull<Stmt>();
}

std::string GenerateConstructs::makeRegionLabel(FunctorDecl *fn, const std::string &suffix) {
  return frmt("{}{}_{}", fn->getIdentifier(), suffix, labels[fn]);
}

void GenerateConstructs::addHostAPI() {
}

void GenerateConstructs::addDeviceAPI() {
  ASTApi builder { CI.getContext() };
//  builder.AddToContext(APIModule,
//      builder.CreateExternFunction("__tlang_device_malloc", builder.CreateType<AddressType>(),
//          builder.CreateParameter("kind", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)),
//          builder.CreateParameter("size", builder.CreateType<IntType>(IntType::P_64, IntType::Unsigned))));
//  builder.AddToContext(APIModule,
//      builder.CreateExternFunction("__tlang_device_free", builder.CreateType<AddressType>(),
//          builder.CreateParameter("address", builder.CreateType<AddressType>())));
  builder.AddToContext(APIModule,
      builder.CreateExternFunction("__tlang_device_map", builder.CreateType<AddressType>(),
          builder.CreateParameter("kind", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)),
          builder.CreateParameter("address", builder.CreateType<AddressType>()),
          builder.CreateParameter("size", builder.CreateType<IntType>(IntType::P_64, IntType::Unsigned))));
  builder.AddToContext(APIModule,
      builder.CreateExternFunction("__tlang_device_get_mapped", builder.CreateType<AddressType>(),
          builder.CreateParameter("address", builder.CreateType<AddressType>())));
  builder.AddToContext(APIModule,
      builder.CreateExternFunction("__tlang_device_sync", builder.CreateVoid(),
          builder.CreateParameter("id", builder.CreateType<IntType>(IntType::P_32, IntType::Signed))));
}

void GenerateConstructs::addAPI(ParallelConstructDatabase &constructs) {
  if (!APIModule) {
    APIModule = CI.getContext().make<ModuleDecl>(NamedDecl("tlang.api.functions"), DeclContext());
    CI.getContext().addModule(unit, APIModule);
    if (constructs.deviceQ())
      addDeviceAPI();
    if (constructs.hostQ())
      addHostAPI();
  }
}

FunctionDecl* GenerateConstructs::generateRegion(ConstructData<ParallelStmt> region, ContextStmt::Kind kind) {
  ASTApi builder { CI.getContext() };
  bool isDevice = kind == ContextStmt::Device;
  List<ParameterDecl*> arguments;
  if (auto opts = region.construct.node->getParallelOptions()) {
    for (auto &expr : opts->getFirstPrivateVariables())
      if (auto re = dyn_cast<DeclRefExpr>(expr))
        if (auto vd = dyn_cast<VariableDecl>(re->getDecl().data()))
          arguments.push_back(builder.CreateParameter(vd->getIdentifier(), vd->getType().getCanonicalType()));
    for (auto &expr : opts->getSharedVariables())
      if (auto re = dyn_cast<DeclRefExpr>(expr))
        if (auto vd = dyn_cast<VariableDecl>(re->getDecl().data()))
          arguments.push_back(builder.CreateParameter(vd->getIdentifier() + "_shared", QualType(vd->getType())));
  }
  auto fn = builder.CreateFunction(makeRegionLabel(region.functor, "_kernel"), builder.CreateVoid(), std::move(arguments));
  auto &body = fn->getBody();
  if (auto opts = region.construct.node->getParallelOptions()) {
    if (opts->getSharedVariables().size() || opts->getPrivateVariables().size()) {
      auto declStmt = builder.CreateDeclStmt(List<VariableDecl*> { });
      auto &declGroup = declStmt->getDecl();
      for (auto &expr : opts->getSharedVariables()) {
        if (auto re = dyn_cast<DeclRefExpr>(expr))
          if (auto vd = dyn_cast<VariableDecl>(re->getDecl().data())) {
            auto parameter = fn->find(vd->getIdentifier() + "_shared");
            Expr *init = builder.CreateUnOp(UnaryOperator::Dereference, builder.CreateDeclRefExpr(dyn_cast<ValueDecl>(*parameter)));
            VariableDecl *var = builder.CreateVariable(vd->getIdentifier(), vd->getType().modQuals(QualType::Reference), init);
            declGroup.push_back(var);
          }
      }
      for (auto &expr : opts->getPrivateVariables()) {
        if (auto re = dyn_cast<DeclRefExpr>(expr))
          if (auto vd = dyn_cast<VariableDecl>(re->getDecl().data())) {
            VariableDecl *var = builder.CreateVariable(vd->getIdentifier(), vd->getType());
            declGroup.push_back(var);
          }
      }
      builder.AppendStmt(body, declStmt);
      builder.AppendStmt(body, region.construct.node->getStmt());
    }
  }
  return fn;
}

void GenerateConstructs::generateHostRegion(ConstructData<ParallelStmt> region) {
  ASTApi builder { CI.getContext() };
  auto fn = generateRegion(region, ContextStmt::Host);
  builder.AddToContext(hostModule, fn);
  regions[region.construct.node] = fn;
  auto lfn = builder.CreateExternFunction(makeRegionLabel(region.functor, "launch"), builder.CreateVoid());
  builder.AddToContext(APIModule, lfn);
  region.construct.reference.makeNull<Stmt>();
  incrementRegionLabel(region.functor);
}

void GenerateConstructs::generateDeviceRegion(ConstructData<ParallelStmt> region) {
  ASTApi builder { CI.getContext() };
  auto fn = generateRegion(region, ContextStmt::Device);
  builder.AddToContext(deviceModule, fn);
  fn->getFunctionKind() = FunctionDecl::Kernel;
  regions[region.construct.node] = fn;
  auto lfn = builder.CreateExternFunction(makeRegionLabel(region.functor, "launch"), builder.CreateVoid());
  builder.AddToContext(APIModule, lfn);
  region.construct.reference.makeNull<Stmt>();
  incrementRegionLabel(region.functor);
}

void GenerateConstructs::generateParallelRegions(ParallelConstructDatabase &constructs) {
  if (constructs.hostQ()) {
    hostModule = CI.getContext().make<ModuleDecl>(NamedDecl("tlang.host.functions"), DeclContext());
    CI.getContext().addModule(unit, hostModule);
  }
  if (constructs.deviceQ()) {
    deviceModule = CI.getContext().make<ModuleDecl>(NamedDecl("tlang.device.functions"), DeclContext());
    deviceUnit = CI.getContext().addUnit("tlang.device", UnitDecl::NVPTX);
    CI.getContext().addModule(deviceUnit, deviceModule);
  }
  for (auto &region : constructs.deviceRegions)
    generateDeviceRegion(region);
  for (auto &region : constructs.hostRegions)
    generateHostRegion(region);
}

//struct ConstructTransformVisitor: public ASTVisitor<ConstructTransformVisitor, VisitorPattern::preOrder> {
//
//};

void GenerateConstructs::generateContext(ConstructData<ContextStmt> context) {
  auto ctx = context.construct.node;
  if (ctx->getContextKind() == ContextStmt::Device) {
    auto fn = dyn_cast<FunctorDecl>(APIModule->find("__tlang_device_map").get().get());
    ASTApi builder { CI.getContext() };
    auto cs = builder.CreateCompoundStmt();
    context.construct.reference.assign<Stmt>(cs);
    for (auto ms : ctx->getMappedExprs()) {
      if (ms->getMapKind() == MapStmt::destroy || ms->getMapKind() == MapStmt::from)
        continue;
      for (auto me : ms->getMappedExprs()) {
        cs->addStmt(builder.CreateCallExpr(builder.CreateDeclRefExpr(fn), builder.CreateLiteral((int64_t) ms->getMapKind())));
      }
    }
    cs->addStmt(ctx->getStmt());
  } else
    context.construct.reference.assign<Stmt>(ctx->getStmt());
}

void GenerateConstructs::generateContexts(ParallelConstructDatabase &constructs) {
  for (auto ctx : constructs.contexts)
    generateContext(ctx);
}
}
