#include <AST/Api.hh>
#include <AST/Visitors/ASTVisitor.hh>
#include <Parallel/GenerateConstructs.hh>
#include <Sema/Sema.hh>

namespace tlang {
bool GenerateConstructs::run(UnitDecl &decl, AnyASTNodeRef ref, ResultManager &results) {
  unit = &decl;
  auto constructs = results.getResult<ParallelConstructDatabase>(CreateConstructDatabase::ID(), &decl);
  if (constructs) {
    print(std::cerr, "Transforming parallel constructs\n");
    addAPI(*constructs);
    collectDeviceCalls(*constructs);
    generateParallelRegions(*constructs);
    generateContexts(*constructs);
    Sema { CI }.resolveSymbolTables(&decl);
    generateLaunchCalls(*constructs);
    Sema { CI }.resolveSymbolTables(&decl);
    duplicateFunctionCalls();
    if (deviceUnit)
      Sema { CI }.resolveSymbolTables(deviceUnit);
    Sema { CI }.resolveNames(&decl);
    if (deviceUnit)
      Sema { CI }.resolveNames(deviceUnit);
    print(std::cerr, "Finished transforming parallel constructs\n");
  }
  return true;
}

std::string GenerateConstructs::makeRegionLabel(FunctorDecl *fn, const std::string &suffix) {
  return frmt("{0}{1}_{2}", fn->getIdentifier(), suffix, labels[fn]);
}

void GenerateConstructs::addAPI(ParallelConstructDatabase &constructs) {
  if (!APIModule) {
    APIModule = CI.getContext().make<ModuleDecl>(NamedDecl("tlang.api.functions"), DeclContext());
    CI.getContext().addModule(unit, APIModule);
    if (constructs.hostQ())
      addHostAPI();
    if (constructs.deviceQ())
      addDeviceAPI();
  }
}

void GenerateConstructs::generateRegionParameters(ConstructData<ParallelStmt> region, ContextStmt::Kind kind,
    List<ParameterDecl*> &parameters) {
  ASTApi builder { CI.getContext() };
  bool isDevice = kind == ContextStmt::Device;
  if (auto opts = region.construct.node->getParallelOptions()) {
    for (auto &expr : opts->getFirstPrivateVariables())
      if (auto re = dyn_cast<DeclRefExpr>(expr))
        if (auto vd = dyn_cast<VariableDecl>(re->getDecl().data())) {
          QualType type = vd->getType();
          if (type.isReference())
            type = type.modQuals();
          if (isa<PtrType>(vd->getType().getType()) && isDevice)
            type = type.changeAS(1);
          parameters.push_back(builder.CreateParameter(vd->getIdentifier(), type));
        }
    for (auto &expr : opts->getSharedVariables())
      if (auto re = dyn_cast<DeclRefExpr>(expr))
        if (auto vd = dyn_cast<VariableDecl>(re->getDecl().data())) {
          QualType type = builder.CreateType<PtrType>(vd->getType().getType());
          if (isDevice)
            type = type.changeAS(1);
          parameters.push_back(builder.CreateParameter(vd->getIdentifier() + "_shared", type));
        }
  }
}

List<ParameterDecl*> GenerateConstructs::generateLaunchParameters(ConstructData<ParallelStmt> region, ContextStmt::Kind kind) {
  ASTApi builder { CI.getContext() };
  bool isDevice = kind == ContextStmt::Device;
  List<ParameterDecl*> parameters;
  if (isDevice) {
    parameters.push_back(builder.CreateParameter("id", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)));
    parameters.push_back(builder.CreateParameter("tx", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)));
    parameters.push_back(builder.CreateParameter("ty", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)));
    parameters.push_back(builder.CreateParameter("tz", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)));
    parameters.push_back(builder.CreateParameter("mx", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)));
    parameters.push_back(builder.CreateParameter("my", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)));
    parameters.push_back(builder.CreateParameter("mz", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)));
  } else
    parameters.push_back(builder.CreateParameter("num_threads", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)));
  generateRegionParameters(region, ContextStmt::Host, parameters);
  return parameters;
}

List<ParameterDecl*> GenerateConstructs::generateRegionParameters(ConstructData<ParallelStmt> region, ContextStmt::Kind kind) {
  List<ParameterDecl*> parameters;
  generateRegionParameters(region, kind, parameters);
  return parameters;
}

FunctionDecl* GenerateConstructs::generateRegion(ConstructData<ParallelStmt> region, ContextStmt::Kind kind) {
  ASTApi builder { CI.getContext() };
  bool isDevice = kind == ContextStmt::Device;

  auto lfn = builder.CreateExternFunction(makeRegionLabel(region.functor, "_launch"), builder.CreateVoid(),
      generateLaunchParameters(region, kind));
  builder.AddToContext(APIModule, lfn);
  launchFunctions[region.construct.node] = lfn;

  auto fn = builder.CreateFunction(makeRegionLabel(region.functor, "_kernel"), builder.CreateVoid(),
      generateRegionParameters(region, kind));
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
    }
  }
  builder.AppendStmt(body, region.construct.node->getStmt());
  return fn;
}

void GenerateConstructs::generateParallelRegions(ParallelConstructDatabase &constructs) {
  ASTApi builder { CI.getContext() };
  cxxModule = CI.getContext().make<ModuleDecl>(NamedDecl("tlang.rt.functions"), DeclContext());
  cxxUnit = CI.getContext().addUnit("tlang.rt", UnitDecl::CXX);
  CI.getContext().addModule(cxxUnit, cxxModule);
  if (constructs.hostQ()) {
    hostModule = CI.getContext().make<ModuleDecl>(NamedDecl("tlang.host.functions"), DeclContext());
    CI.getContext().addModule(unit, hostModule);
    cxxModule->add(builder.CreateCXX("#include <tlang-host.h>\n"));
  }
  if (constructs.deviceQ()) {
    auto target = CI.getOptions().langOpts.deviceTarget;
    deviceModule = CI.getContext().make<ModuleDecl>(NamedDecl("tlang.device.functions"), DeclContext());
    deviceUnit = CI.getContext().addUnit("tlang.device", UnitDecl::NVPTX);
    CI.getContext().addModule(deviceUnit, deviceModule);
    cxxModule->add(
        builder.CreateCXX(
            "#include <tlang-device-nvptx.h>\n\nstatic int __tlang_device_module_status = __tlang_device_load_module(\"tlang.device.devbin\");\n"));
  }
  for (auto &region : constructs.deviceRegions)
    generateDeviceRegion(region);
  for (auto &region : constructs.hostRegions)
    generateHostRegion(region);
}

void GenerateConstructs::addMappings(CompoundStmt *stmt, ContextStmt *context, int pass) {
  ASTApi builder { CI.getContext() };
  auto &mapped = context->getMappedExprs();
  if (mapped.size()) {
    auto mapfn = APIModule->find("__tlang_device_map").get().get<ExternFunctionDecl>();
    assert(mapfn);
    for (auto mstmt : mapped) {
      auto mk = mstmt->getMapKind();
      auto &me = mstmt->getMappedExprs();
      for (auto expr : me) {
        Expr *address { }, *size { }, *map_expr { };
        if (auto dre = dyn_cast<DeclRefExpr>(expr)) {
          address = builder.CreateUnOp(UnaryOperator::Address, dre);
          size = builder.CreateLiteral(dre->getType().getType()->getSizeOf(), IntType::P_64);
        } else if (auto ae = dyn_cast<ArrayExpr>(expr)) {
          address = ae->getArray();
          size = builder.CreateBinOp(BinaryOperator::Multiply, builder.CreateLiteral(ae->getType().getType()->getSizeOf(), IntType::P_64),
              ae->getIndex()[0]);
        }
        assert(address);
        assert(size);
        if ((mk & MapStmt::to) == MapStmt::to && pass == 0) {
          map_expr = builder.CreateCallExpr(builder.CreateDeclRefExpr(mapfn), builder.CreateLiteral((int64_t) MapStmt::to, IntType::P_32),
              address, size);
        } else if ((mk & MapStmt::from) == MapStmt::from && pass == 1) {
          map_expr = builder.CreateCallExpr(builder.CreateDeclRefExpr(mapfn), builder.CreateLiteral((int64_t) MapStmt::from, IntType::P_32),
              address, size);
        } else if (mk == MapStmt::present && pass == 0) {
          map_expr = builder.CreateCallExpr(builder.CreateDeclRefExpr(mapfn), builder.CreateLiteral((int64_t) mk, IntType::P_32), address,
              size);
        } else if (mk == MapStmt::create && pass == 0) {
          map_expr = builder.CreateCallExpr(builder.CreateDeclRefExpr(mapfn), builder.CreateLiteral((int64_t) mk, IntType::P_32), address,
              size);
        } else if (mk == MapStmt::destroy && pass == 0) {
          map_expr = builder.CreateCallExpr(builder.CreateDeclRefExpr(mapfn), builder.CreateLiteral((int64_t) mk, IntType::P_32), address,
              size);
        }
        if (map_expr)
          stmt->addStmt(map_expr);
      }
    }
  }
}

void GenerateConstructs::generateContext(ConstructData<ContextStmt> context) {
  auto context_construct = context.construct.node;
  if (auto ic = dyn_cast<ImplicitContextStmt>(context_construct))
    implicitRefs[ic] = context.construct.reference;
  ASTApi builder { CI.getContext() };
  auto cs = builder.CreateCompoundStmt();
  contextCS[context_construct] = cs;
  context.construct.reference.assign<Stmt>(cs);
  addMappings(cs, context_construct, 0);
  cs->addStmt(context_construct->getStmt());
  addMappings(cs, context_construct, 1);
}

void GenerateConstructs::generateContexts(ParallelConstructDatabase &constructs) {
  for (auto ctx : constructs.contexts)
    generateContext(ctx);
}

void GenerateConstructs::generateLaunchCalls(ParallelConstructDatabase &constructs) {
  for (auto &region : constructs.deviceRegions)
    generateDeviceLaunch(region);
  for (auto &region : constructs.hostRegions)
    generateHostLaunch(region);
}

namespace {
struct CallExprVisitor: ASTVisitor<CallExprVisitor, VisitorPattern::preOrder> {
  CallExprVisitor(std::set<FunctorDecl*> &callsInDeviceCode) :
      callsInDeviceCode(callsInDeviceCode) {
  }
  visit_t visitCallExpr(CallExpr *expr) {
    if (auto re = dyn_cast<DeclRefExpr>(expr->getCallee())) {
      if (auto fd = dyn_cast<FunctorDecl>(re->getDecl().data())) {
        callsInDeviceCode.insert(fd);
        if (isa<FunctionDecl>(fd))
          if (auto dc = static_cast<DeclContext*>(fd->getDeclContext().data())) {
            if (auto symbol = dc->find(fd->getIdentifier()))
              dc->erase(symbol);
            else
              assert(false);
          } else
            assert(false);
      } else
        assert(false);
    } else
      assert(false);
    return visit;
  }
  std::set<FunctorDecl*> &callsInDeviceCode;
};
}

void GenerateConstructs::collectDeviceCalls(ParallelConstructDatabase &constructs) {
  for (auto construct : constructs.deviceRegions)
    CallExprVisitor { callsInDeviceCode }.dynamicTraverse(construct.construct.node->getStmt());
}

void GenerateConstructs::duplicateFunctionCalls() {
  ASTApi builder { CI.getContext() };
  for (auto fd : callsInDeviceCode) {
    if (fd) {
      if (isa<FunctionDecl>(fd))
        builder.AddToContext(deviceModule, fd);
      else {
        ExternFunctionDecl *cpy = CI.getContext().make<ExternFunctionDecl>();
        cpy->getIdentifier() = fd->getIdentifier();
        cpy->getReturnType() = fd->getReturnType();
        cpy->getType() = fd->getType();
        cpy->getParameters() = fd->getParameters();
        cpy->addArgs(List<ParameterDecl*>(fd->getParameters()));
//        std::cerr << *cpy << std::endl;
        builder.AddToContext(deviceModule, cpy);
      }
    }
  }
}
}
