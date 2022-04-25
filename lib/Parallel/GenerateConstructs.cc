#include <AST/Api.hh>
#include <AST/Visitors/ASTVisitor.hh>
#include <Parallel/GenerateConstructs.hh>
#include <Sema/Sema.hh>

namespace tlang {
bool GenerateConstructs::run(UnitDecl &decl, AnyASTNodeRef ref, ResultManager &results) {
  unit = &decl;
  auto constructs = results.getResult<ParallelConstructDatabase>(CreateConstructDatabase::ID(), &decl);
  if (constructs) {
    print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::yellow_green), "Transforming parallel constructs\n");
    addAPI(*constructs);
    generateParallelRegions(*constructs);
    generateContexts(*constructs);
    Sema { CI }.resolveSymbolTables(&decl);
    generateLaunchCalls(*constructs);
    print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::lime_green), "Finished transforming parallel constructs\n");
  }
  return true;
}

std::string GenerateConstructs::makeRegionLabel(FunctorDecl *fn, const std::string &suffix) {
  return frmt("{}{}_{}", fn->getIdentifier(), suffix, labels[fn]);
}

void GenerateConstructs::addHostAPI() {
}

void GenerateConstructs::addDeviceAPI() {
  ASTApi builder { CI.getContext() };
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
  auto lfn = builder.CreateExternFunction(makeRegionLabel(region.functor, "_launch"), builder.CreateVoid());
  builder.AddToContext(APIModule, lfn);
  launchFunctions[region.construct.node] = lfn;
  region.construct.reference.makeNull<Stmt>();
  incrementRegionLabel(region.functor);
}

void GenerateConstructs::generateDeviceRegion(ConstructData<ParallelStmt> region) {
  ASTApi builder { CI.getContext() };
  auto fn = generateRegion(region, ContextStmt::Device);
  builder.AddToContext(deviceModule, fn);
  fn->getFunctionKind() = FunctionDecl::Kernel;
  regions[region.construct.node] = fn;
  auto lfn = builder.CreateExternFunction(makeRegionLabel(region.functor, "_launch"), builder.CreateVoid());
  builder.AddToContext(APIModule, lfn);
  launchFunctions[region.construct.node] = lfn;
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

namespace {
Stmt* mapToDevice(MapStmt::Kind mapKind, Expr *exprToMap, FunctorDecl *mapFn, ASTApi &builder) {
  Expr *address { };
  Expr *kind { };
  Expr *size { };
  QualType type;
  std::string id { };
  if (auto ae = dyn_cast<ArrayExpr>(exprToMap)) {
    address = builder.CreateImplicitCast(ae->getArray(), builder.CreateAddressType());
    size = builder.CreateBinOp(BinaryOperator::Multiply, ae->getIndex().at(0),
        builder.CreateLiteral((int64_t) exprToMap->getType().getType()->getSizeOf()));
    if (auto re = dyn_cast<DeclRefExpr>(ae->getArray())) {
      id = re->getIdentifier();
      type = ae->getArray()->getType().modQuals();
    } else
      assert(false);
  } else if (exprToMap->getType().isReference()) {
    address = builder.CreateImplicitCast(builder.CreateUnOp(UnaryOperator::Address, exprToMap), builder.CreateAddressType());
    size = builder.CreateLiteral((int64_t) exprToMap->getType().getType()->getSizeOf());
    if (auto re = dyn_cast<DeclRefExpr>(exprToMap)) {
      id = re->getIdentifier();
      type = builder.CreateType<PtrType>(re->getType().getType());
    } else
      assert(false);
  } else
    assert(false);
  kind = builder.CreateLiteral(static_cast<int64_t>(mapKind));
  Expr *cexpr = builder.CreateCallExpr(builder.CreateDeclRefExpr(mapFn), kind, address, size);
  if (mapKind == MapStmt::destroy || mapKind == MapStmt::from)
    return cexpr;
  return builder.CreateDeclStmt(builder.CreateVariable(id + ".mapped", type, builder.CreateImplicitCast(cexpr, type)));
}
}

void GenerateConstructs::generateContext(ConstructData<ContextStmt> context) {
  auto context_construct = context.construct.node;
  if (auto ic = dyn_cast<ImplicitContextStmt>(context_construct))
    implicitRefs[ic] = context.construct.reference;
  if (context_construct->getContextKind() == ContextStmt::Device) {
    auto mapFn = dyn_cast<FunctorDecl>(APIModule->find("__tlang_device_map").get().get());
    ASTApi builder { CI.getContext() };
    auto cs = builder.CreateCompoundStmt();
    contextCS[context_construct] = cs;
    std::cerr << cs << std::endl;
    std::cerr << static_cast<UniversalContext*>(cs)->getParent() << std::endl;
    context.construct.reference.assign<Stmt>(cs);
    for (auto ms : context_construct->getMappedExprs()) {
      if (ms->isDestroy() || ms->isFrom())
        continue;
      for (auto me : ms->getMappedExprs())
        cs->addStmt(mapToDevice(ms->getMapKind() == MapStmt::toFrom ? MapStmt::to : ms->getMapKind(), me, mapFn, builder));
    }
    cs->addStmt(context_construct->getStmt());
    for (auto ms : context_construct->getMappedExprs()) {
      if (ms->isDestroy() || ms->isFrom() || ms->isToFrom())
        for (auto me : ms->getMappedExprs())
          cs->addStmt(mapToDevice(ms->getMapKind() == MapStmt::toFrom ? MapStmt::from : ms->getMapKind(), me, mapFn, builder));
      else
        continue;
    }
  } else
    context.construct.reference.assign<Stmt>(context_construct->getStmt());
}

void GenerateConstructs::generateContexts(ParallelConstructDatabase &constructs) {
  for (auto ctx : constructs.contexts)
    generateContext(ctx);
}

void GenerateConstructs::generateHostLaunch(ConstructData<ParallelStmt> region) {
  ASTApi builder { CI.getContext() };
  List<Expr*> arguments;
  if (auto opts = region.construct.node->getParallelOptions()) {
    for (auto &expr : opts->getFirstPrivateVariables())
      arguments.push_back(expr);
    for (auto &expr : opts->getSharedVariables())
      arguments.push_back(builder.CreateUnOp(UnaryOperator::Address, expr));
  }
  ExternFunctionDecl *lfn = launchFunctions[region.construct.node];
  assert(lfn);
  AnyASTNodeRef ref = implicitRefs[dyn_cast<ImplicitContextStmt>(region.construct.node->getContext().data())];
  assert(ref);
  ref.assign<Stmt>(builder.CreateCallExpr(builder.CreateDeclRefExpr(lfn), std::move(arguments)));
}
void GenerateConstructs::generateDeviceLaunch(ConstructData<ParallelStmt> region) {
  ASTApi builder { CI.getContext() };
  List<Expr*> arguments;
  auto cs = contextCS[dyn_cast<ContextStmt>(region.construct.node->getContext().data())];
  assert(cs);
  if (auto opts = region.construct.node->getParallelOptions()) {
    for (auto &expr : opts->getFirstPrivateVariables()) {
      auto re = dyn_cast<DeclRefExpr>(expr);
      if (auto pt = dyn_cast<PtrType>(re->getType().getType())) {
        auto mv = cs->find(re->getIdentifier() + ".mapped", false);
        assert(mv);
        arguments.push_back(builder.CreateDeclRefExpr(dyn_cast<VariableDecl>(mv.get())));
      } else
        arguments.push_back(expr);
    }
    for (auto &expr : opts->getSharedVariables()) {
      auto re = dyn_cast<DeclRefExpr>(expr);
      auto mv = cs->find(re->getIdentifier() + ".mapped", false);
      assert(mv);
      arguments.push_back(builder.CreateDeclRefExpr(dyn_cast<VariableDecl>(mv.get())));
    }
  }
  ExternFunctionDecl *lfn = launchFunctions[region.construct.node];
  assert(lfn);
//  AnyASTNodeRef ref = implicitRefs[dyn_cast<ImplicitContextStmt>(region.construct.node->getContext().data())];
//  assert(ref);
  cs->addStmt(builder.CreateCallExpr(builder.CreateDeclRefExpr(lfn), std::move(arguments)));
}
void GenerateConstructs::generateLaunchCalls(ParallelConstructDatabase &constructs) {
  for (auto &region : constructs.deviceRegions)
    generateDeviceLaunch(region);
  for (auto &region : constructs.hostRegions)
    generateHostLaunch(region);
}
}
