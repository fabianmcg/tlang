#ifndef PARALLEL_GENERATECONSTRUCTS_HH
#define PARALLEL_GENERATECONSTRUCTS_HH

#include <Parallel/CreateConstructDatabase.hh>

namespace tlang {
class GenerateConstructs: public CompilerInvocationRef, public PassBase<GenerateConstructs> {
public:
  using CompilerInvocationRef::CompilerInvocationRef;
  bool run(UnitDecl &decl, AnyASTNodeRef ref, ResultManager &results);
  static PassID* ID() {
    static PassID pid { };
    return &pid;
  }
  void addHostAPI();
  void addDeviceAPI();
  void addAPI(ParallelConstructDatabase &constructs);
  void generateContext(ConstructData<ContextStmt> context);
  void generateContexts(ParallelConstructDatabase &constructs);
  void generateRegionParameters(ConstructData<ParallelStmt> region, ContextStmt::Kind kind, List<ParameterDecl*>& parameters);
  List<ParameterDecl*> generateLaunchParameters(ConstructData<ParallelStmt> region, ContextStmt::Kind kind);
  List<ParameterDecl*> generateRegionParameters(ConstructData<ParallelStmt> region, ContextStmt::Kind kind);
  FunctionDecl* generateRegion(ConstructData<ParallelStmt> region, ContextStmt::Kind kind);
  void generateHostRegion(ConstructData<ParallelStmt> region);
  void generateDeviceRegion(ConstructData<ParallelStmt> region);
  void generateParallelRegions(ParallelConstructDatabase &constructs);
  void generateHostLaunch(ConstructData<ParallelStmt> region);
  void generateDeviceLaunch(ConstructData<ParallelStmt> region);
  void generateLaunchCalls(ParallelConstructDatabase &constructs);
protected:
  UnitDecl *unit { };
  ModuleDecl *APIModule { };
  ModuleDecl *hostModule { };
  ModuleDecl *deviceModule { };
  ModuleDecl *cxxModule { };
  UnitDecl *deviceUnit { };
  UnitDecl *cxxUnit { };
  std::map<ParallelStmt*, FunctionDecl*> regions;
  std::map<ParallelStmt*, ExternFunctionDecl*> launchFunctions;
  std::map<ImplicitContextStmt*, AnyASTNodeRef> implicitRefs;
  std::map<ContextStmt*, CompoundStmt*> contextCS;
  std::map<FunctorDecl*, int> labels;
  std::string makeRegionLabel(FunctorDecl *fn, const std::string &suffix = "");
  int incrementRegionLabel(FunctorDecl *fn) {
    return labels[fn]++;
  }
};
}

#endif
