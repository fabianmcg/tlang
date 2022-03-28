#ifndef __EMITTERS_EMITTER_HH__
#define __EMITTERS_EMITTER_HH__

#include <llvm/TableGen/TableGenBackend.h>
#include <Constants.hh>
#include <TGUtility.hh>
#include <Utility.hh>

enum class EmitMode {
  header,
  source,
};

namespace llvm {
class RecordKeeper;
class Record;
class RecordVal;
class raw_ostream;
}

class Emitter {
protected:
  llvm::RecordKeeper &records;
  EmitMode mode;
public:
  Emitter(llvm::RecordKeeper &records, EmitMode mode) :
      records(records), mode(mode) {
  }
  ~Emitter() = default;
  void run(llvm::raw_ostream &ost);
};

#endif
