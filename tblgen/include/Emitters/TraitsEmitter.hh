#ifndef __TRAITS_EMITTER_HH__
#define __TRAITS_EMITTER_HH__

#include "Emitter.hh"

class TraitsEmitter: public Emitter {
public:
  using Emitter::Emitter;
  void run(llvm::raw_ostream &ost);
};

#endif
