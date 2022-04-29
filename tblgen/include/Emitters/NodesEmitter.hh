#ifndef __NODES_EMITTER_HH__
#define __NODES_EMITTER_HH__

#include "Emitter.hh"

class NodesEmitter: public Emitter {
public:
  using Emitter::Emitter;
  void emitHeader(llvm::raw_ostream &ost);
  void emitSource(llvm::raw_ostream &ost);
  void run(llvm::raw_ostream &ost) {
    if (mode == EmitMode::source)
      emitSource(ost);
    else if (mode == EmitMode::header)
      emitHeader(ost);
  }
};

#endif
