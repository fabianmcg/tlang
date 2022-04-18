//#include <Transformation/ParallelPipeline.hh>
//#include <Passes/PassManager.hh>
//
//#include "ParallelPipeline/AddGpuFunctions.hh"
//#include "ParallelPipeline/Analysis.hh"
//#include "ParallelPipeline/ParallelCodeGen.hh"
//
//namespace tlang {
//void ParallelPipeline::init() {
//  ModulePM MPM;
//  MPM.addPass(ParallelAnalysisPass(context));
//  MPM.addPass(AddGPUFunctions(context));
//  MPM.addPass(ParallelCodeGen(context));
//  addPass(impl::makePassAdaptor<ModuleDecl, UniversePM>(std::move(MPM)));
//}
//PassID* ParallelPipeline::ID() {
//  static PassID pid { };
//  return &pid;
//}
//}
