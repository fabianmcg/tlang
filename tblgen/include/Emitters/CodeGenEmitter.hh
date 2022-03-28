#ifndef __VISITOR_EMITTER_HH__
#define __VISITOR_EMITTER_HH__

#include "Emitter.hh"

class CodeGenEmitter: public Emitter {
public:
  using Emitter::Emitter;
  void run(llvm::raw_ostream &ost);
};

#endif
