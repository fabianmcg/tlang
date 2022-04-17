#ifndef PP_AGPU_HH
#define PP_AGPU_HH
#include <AST/Api.hh>
#include <Transformation/ParallelPipeline.hh>

namespace tlang {
struct AddGPUFunctions: public ASTContextReference, public PassBase<AddGPUFunctions> {
  using ASTContextReference::ASTContextReference;
  bool run(ModuleDecl &module, AnyASTNodeRef nodeRef, ResultManager &manager) {
    std::cerr << name().str() << "Running" << std::endl;
    builder.AddToContext(&module, builder.CreateExternFunction("__tlang_gpu_create_context", QualType()));
    builder.AddToContext(&module, builder.CreateExternFunction("__tlang_gpu_destroy_context", QualType()));
    builder.AddToContext(&module,
        builder.CreateExternFunction("__tlang_gpu_malloc", builder.CreateType<AddressType>(),
            builder.CreateParameter("kind", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)),
            builder.CreateParameter("size", builder.CreateType<IntType>(IntType::P_64, IntType::Unsigned))));
    builder.AddToContext(&module,
        builder.CreateExternFunction("__tlang_gpu_free", builder.CreateType<AddressType>(),
            builder.CreateParameter("address", builder.CreateType<AddressType>())));
    builder.AddToContext(&module,
        builder.CreateExternFunction("__tlang_gpu_copy", builder.CreateType<AddressType>(),
            builder.CreateParameter("kind", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)),
            builder.CreateParameter("dest", builder.CreateType<AddressType>()),
            builder.CreateParameter("src", builder.CreateType<AddressType>()),
            builder.CreateParameter("size", builder.CreateType<IntType>(IntType::P_64, IntType::Unsigned))));
    builder.AddToContext(&module,
        builder.CreateExternFunction("__tlang_gpu_map", builder.CreateType<AddressType>(),
            builder.CreateParameter("kind", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)),
            builder.CreateParameter("address", builder.CreateType<AddressType>()),
            builder.CreateParameter("size", builder.CreateType<IntType>(IntType::P_64, IntType::Unsigned))));
    builder.AddToContext(&module,
        builder.CreateExternFunction("__tlang_gpu_get_mapped", builder.CreateType<AddressType>(),
            builder.CreateParameter("address", builder.CreateType<AddressType>())));
    builder.AddToContext(&module,
        builder.CreateExternFunction("__tlang_gpu_launch", builder.CreateType<AddressType>(),
            builder.CreateParameter("kernel", builder.CreateType<AddressType>()),
            builder.CreateParameter("gx", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)),
            builder.CreateParameter("gy", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)),
            builder.CreateParameter("gz", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)),
            builder.CreateParameter("bx", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)),
            builder.CreateParameter("by", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)),
            builder.CreateParameter("bz", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)),
            builder.CreateParameter("args", builder.CreateType<PtrType>(builder.CreateType<AddressType>().getType())),
            builder.CreateParameter("sm", builder.CreateType<IntType>(IntType::P_64, IntType::Unsigned))));
    return true;
  }
  static PassID* ID() {
    static PassID pid { };
    return &pid;
  }
  ASTApi builder { context };
};
}

#endif
