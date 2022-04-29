#ifndef __INCFILE_EMITTER_HH__
#define __INCFILE_EMITTER_HH__

#include "Emitter.hh"

class IncFileEmitter: public Emitter {
public:
  using Emitter::Emitter;
  void run(llvm::raw_ostream &ost);
};

#endif
