#ifndef __COMMON_HH__
#define __COMMON_HH__

#include <llvm/TableGen/Record.h>

inline llvm::Record* getAsRecord(const llvm::RecordVal *value) {
  if (value)
    if (llvm::DefInit *di = llvm::dyn_cast<llvm::DefInit>(value->getValue()))
      return di->getDef();
  return nullptr;
}

inline std::string name(const llvm::Record *record) {
  if (record)
    return record->getName().str();
  return "";
}

inline std::string name(const llvm::Record &record) {
  return record.getName().str();
}

#endif
