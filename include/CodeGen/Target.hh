#ifndef CODEGEN_TARGET_HH
#define CODEGEN_TARGET_HH

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace tlang {
struct Target {
  typedef enum {
    Generic,
    NVPTX,
    AMDGPU,
    A64FX
  } TargetArchitecture;

  struct GenericOpts {
  };
  struct NVPTXOpts {
    uint8_t vectorSize = 32;
    uint8_t defaultMatrixSize = 8;
  };
  struct AMDGPUOpts {
    uint8_t vectorSize = 64;
    uint8_t defaultMatrixSize = 4;
  };
  TargetArchitecture arch = TargetArchitecture::Generic;
  union {
    GenericOpts genericOpts;
    NVPTXOpts nvptxOpts;
    AMDGPUOpts amdGPUOpts;
  };
  Target() {
    genericOpts = GenericOpts { };
  }
  Target(TargetArchitecture arch) :
      arch(arch) {
    if (arch == Generic)
      genericOpts = GenericOpts { };
    if (arch == NVPTX)
      nvptxOpts = NVPTXOpts { };
    else if (arch == AMDGPU)
      amdGPUOpts = AMDGPUOpts { };
  }
  template <typename Opts>
  void set(const Opts &options) {
    if constexpr (std::is_same_v<Opts, NVPTXOpts>) {
      arch = NVPTX;
      nvptxOpts = options;
    } else if constexpr (std::is_same_v<Opts, AMDGPUOpts>) {
      arch = AMDGPU;
      amdGPUOpts = options;
    }
  }
};
}

#endif
