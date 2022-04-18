#ifndef FRONTEND_COMPILEROPTIONS_HH
#define FRONTEND_COMPILEROPTIONS_HH

#include <cstdint>
#include <CodeGen/Target.hh>

namespace tlang {
class ParallelLangOpts {
public:
  typedef enum {
    Host,
    Device
  } ContextKind;
  bool parallelize = true;
  ContextKind defaultContext = Device;
  Target hostTarget = { };
  Target deviceTarget = { Target::NVPTX };
};
class CompilerOptions {
public:
  CompilerOptions() = default;
  ParallelLangOpts langOpts { };
};
}

#endif
