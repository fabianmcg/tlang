#ifndef __COMMON_EMITTER_HH__
#define __COMMON_EMITTER_HH__

#include "Emitter.hh"

class CommonEmitter: public Emitter {
public:
  using Emitter::Emitter;
  void run(llvm::raw_ostream &ost);
};

#endif
