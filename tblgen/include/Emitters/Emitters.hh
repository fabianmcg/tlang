#ifndef __EMITTERS_HH__
#define __EMITTERS_HH__

#include <llvm/Support/raw_ostream.h>
#include <llvm/TableGen/Record.h>
#include "CodeGenEmitter.hh"
#include "CommonEmitter.hh"
#include "IncFileEmitter.hh"
#include "NodesEmitter.hh"
#include "TraitsEmitter.hh"

template <typename Emitter>
void emit(llvm::RecordKeeper &records, llvm::raw_ostream &ost, EmitMode mode = EmitMode::header) {
  Emitter(records, mode).run(ost);
}

#endif
