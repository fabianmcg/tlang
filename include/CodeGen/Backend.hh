#ifndef CODEGEN_BACKEND_HH
#define CODEGEN_BACKEND_HH

#include <CodeGen/Target.hh>

namespace tlang {
class Backend {
public:
  typedef enum {
    Generic,
    NVPTX,
    AMDGPU,
    CXX
  } Kind;
  Backend() = default;
  Backend(Kind kind) :
      backend(kind) {
    if (backend == NVPTX)
      target.set(Target::NVPTXOpts { });
    else if (backend == AMDGPU)
      target.set(Target::AMDGPUOpts { });
  }
  Kind getBackend() const {
    return backend;
  }
  Target getTarget() const {
    return target;
  }
protected:
  Kind backend = Generic;
  Target target = { };
};
}

#endif
