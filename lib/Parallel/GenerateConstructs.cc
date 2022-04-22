#include <AST/Api.hh>
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
  for (auto &c : constructs.contexts)
    c.construct.reference.makeNull<Stmt>();
}

std::string GenerateConstructs::makeRegionLabel(FunctorDecl *fn, const std::string &suffix) {
  return frmt("{}{}_{}", fn->getIdentifier(), suffix, labels[fn]++);
}

void GenerateConstructs::addHostAPI() {
}

void GenerateConstructs::addDeviceAPI() {
  ASTApi builder { CI.getContext() };
  builder.AddToContext(APIModule,
      builder.CreateExternFunction("__tlang_device_malloc", builder.CreateType<AddressType>(),
          builder.CreateParameter("kind", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)),
          builder.CreateParameter("size", builder.CreateType<IntType>(IntType::P_64, IntType::Unsigned))));
  builder.AddToContext(APIModule,
      builder.CreateExternFunction("__tlang_device_free", builder.CreateType<AddressType>(),
          builder.CreateParameter("address", builder.CreateType<AddressType>())));
  builder.AddToContext(APIModule,
      builder.CreateExternFunction("__tlang_device_map", builder.CreateType<AddressType>(),
          builder.CreateParameter("kind", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)),
          builder.CreateParameter("address", builder.CreateType<AddressType>()),
          builder.CreateParameter("size", builder.CreateType<IntType>(IntType::P_64, IntType::Unsigned))));
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

void GenerateConstructs::generateContext(ConstructData<ContextStmt> context) {
}

void GenerateConstructs::generateContexts(ParallelConstructDatabase &constructs) {
  for (auto ctx : constructs.contexts)
    generateContext(ctx);
}

void GenerateConstructs::generateHostRegion(ConstructData<ParallelStmt> region) {
}

void GenerateConstructs::generateDeviceRegion(ConstructData<ParallelStmt> region) {
  ASTApi builder { CI.getContext() };
  List<ParameterDecl*> arguments;
  if (auto opts = region.construct.node->getParallelOptions()) {
    for (auto &expr : opts->getFirstPrivateVariables()) {
      if (auto re = dyn_cast<DeclRefExpr>(expr))
        if (auto vd = dyn_cast<VariableDecl>(re->getDecl().data())) {
          auto type = vd->getType().getCanonicalType();
          if (isa<PtrType>(type.getType()))
            type = type.changeAS(1);
          arguments.push_back(builder.CreateParameter(vd->getIdentifier(), type));
        }
    }
    for (auto &expr : opts->getSharedVariables()) {
      if (auto re = dyn_cast<DeclRefExpr>(expr))
        if (auto vd = dyn_cast<VariableDecl>(re->getDecl().data())) {
          arguments.push_back(
              builder.CreateParameter(vd->getIdentifier() + "_shared",
                  builder.CreateFQType<PtrType>(QualType::None, 1, vd->getType().getType())));
        }
    }
  }
  auto fn = builder.CreateFunction(makeRegionLabel(region.functor, "_device"), builder.CreateVoid(), std::move(arguments));
  builder.AddToContext(deviceModule, fn);
  fn->getFunctionKind() = FunctionDecl::Kernel;
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
            VariableDecl *var = builder.CreateVariable(vd->getIdentifier(), vd->getType().modQuals(QualType::Reference).changeAS(1), init);
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
}
