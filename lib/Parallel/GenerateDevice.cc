#include <Parallel/GenerateConstructs.hh>
#include <AST/Api.hh>
#include <AST/Visitors/ASTVisitor.hh>
#include <Io/CXX.hh>

namespace tlang {
void GenerateConstructs::addDeviceAPI() {
  ASTApi builder { CI.getContext() };
  if (!APIModule->find("__tlang_device_map")) {
    builder.AddToContext(APIModule,
        builder.CreateExternFunction("__tlang_device_map", builder.CreateType<AddressType>(),
            builder.CreateParameter("kind", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)),
            builder.CreateParameter("address", builder.CreateType<AddressType>()),
            builder.CreateParameter("size", builder.CreateType<IntType>(IntType::P_64, IntType::Signed))));
  }
  if (!APIModule->find("__tlang_device_sync")) {
    builder.AddToContext(APIModule,
        builder.CreateExternFunction("__tlang_device_sync", builder.CreateVoid(),
            builder.CreateParameter("kind", builder.CreateType<IntType>(IntType::P_32, IntType::Signed))));
  }
}

void GenerateConstructs::generateDeviceRegion(ConstructData<ParallelStmt> region) {
  ASTApi builder { CI.getContext() };
  auto fn = generateRegion(region, ContextStmt::Device);
  builder.AddToContext(deviceModule, fn);
  fn->getFunctionKind() = FunctionDecl::Kernel;
  regions[region.construct.node] = fn;
  region.construct.reference.makeNull<Stmt>();
  incrementRegionLabel(region.functor);
}

namespace {
void addLaunchParameters(List<Expr*> &arguments, ConstructData<ParallelStmt> region, ASTApi builder) {
  for (auto te : region.construct.node->getLaunchParameters()) {
    size_t i = 0;
    for (auto ex : te->getElements()) {
      arguments.push_back(ex);
      ++i;
    }
    if (i < 3)
      for (; i < 3; ++i)
        arguments.push_back(builder.CreateLiteral((int64_t) 0, IntType::P_32));
  }
}
}

void GenerateConstructs::generateDeviceLaunch(ConstructData<ParallelStmt> region) {
  ASTApi builder { CI.getContext() };
  List<Expr*> arguments;
  arguments.push_back(builder.CreateLiteral((int64_t) 0, IntType::P_32));
  auto cs = contextCS[dyn_cast<ContextStmt>(region.construct.node->getContext().data())];
  assert(cs);
  addLaunchParameters(arguments, region, builder);
  if (auto opts = region.construct.node->getParallelOptions()) {
    for (auto &expr : opts->getFirstPrivateVariables()) {
      auto re = dyn_cast<DeclRefExpr>(expr);
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
  FunctionDecl *rfn = regions[region.construct.node];
  auto cxxFn = CXXIOVisitor { }.emitExternFunctionDecl(lfn);
  {
    std::string args = "id, Vec3(tx, ty, tz), Vec3(mx, my, mz)";
    auto sz = lfn->getParameters().size();
    for (auto [i, arg] : tlang::enumerate(lfn->getParameters())) {
      if (i > 6)
        args += frmt(", {0}", arg->getIdentifier());
    }
    cxxFn = frmt(
        "{0} {{\n  static void* __kernel = __tlang_device_load_function(\"{1}\");\n  __tlang_device_run_kernel_rt(__kernel, {2});\n}\n",
        cxxFn, rfn->getIdentifier(), args);
  }
  cxxModule->add(builder.CreateCXX(cxxFn));
  assert(lfn);
  cs->addStmt(builder.CreateCallExpr(builder.CreateDeclRefExpr(lfn), std::move(arguments)));
}
}
