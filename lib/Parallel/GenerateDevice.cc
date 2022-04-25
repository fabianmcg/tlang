#include <Parallel/GenerateConstructs.hh>
#include <AST/Api.hh>
#include <AST/Visitors/ASTVisitor.hh>
#include <Io/CXX.hh>

namespace tlang {
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

void GenerateConstructs::generateDeviceRegion(ConstructData<ParallelStmt> region) {
  ASTApi builder { CI.getContext() };
  auto fn = generateRegion(region, ContextStmt::Device);
  builder.AddToContext(deviceModule, fn);
  fn->getFunctionKind() = FunctionDecl::Kernel;
  regions[region.construct.node] = fn;
  region.construct.reference.makeNull<Stmt>();
  incrementRegionLabel(region.functor);
}

void GenerateConstructs::generateDeviceLaunch(ConstructData<ParallelStmt> region) {
  ASTApi builder { CI.getContext() };
  List<Expr*> arguments;
  arguments.push_back(builder.CreateLiteral((int64_t) 0, IntType::P_32));
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
  FunctionDecl *rfn = regions[region.construct.node];
  auto cxxFn = CXXIOVisitor { }.emitExternFunctionDecl(lfn);
  {
    std::string args = frmt("id, Vec3(tx, ty, tz), Vec3(mx, my, mz)");
    auto sz = lfn->getParameters().size();
    for (auto [i, arg] : tlang::enumerate(lfn->getParameters())) {
      if (i > 6)
        args += frmt(", {}", arg->getIdentifier());
    }
    cxxFn = frmt("{} {{\n  static void* __kernel = __tlang_device_load_function(\"{}\");\n  __tlang_device_run_kernel_rt(__kernel, {});\n}}\n", cxxFn, rfn->getIdentifier(), args);
  }
  cxxModule->add(builder.CreateCXX(cxxFn));
  assert(lfn);
  cs->addStmt(builder.CreateCallExpr(builder.CreateDeclRefExpr(lfn), std::move(arguments)));

}
}
