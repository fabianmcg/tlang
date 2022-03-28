#ifndef __INFRA_VARIABLE_HH__
#define __INFRA_VARIABLE_HH__

#include "Visibility.hh"

struct Variable {
  static constexpr std::string_view kind = "Variable";
  llvm::StringRef name;
  llvm::Record &record;
  Variable(llvm::StringRef name, llvm::Record &record) :
      name(name), record(record) {
  }
  static inline bool is(llvm::Record *record) {
    if (record)
      return record->isSubClassOf(kind);
    return false;
  }
  static inline bool is(llvm::RecTy *recTy) {
    if (recTy)
      return recTy->getAsString() == kind;
    return false;
  }
  inline bool is() const {
    return is(&record);
  }
  inline llvm::StringRef type() const {
    return record.getValueAsString("type");
  }
  inline llvm::StringRef init() const {
    return record.getValueAsString("init");
  }
  inline Visibility visibility() const {
    return Visibility(*record.getValueAsDef("visibility"));
  }
  inline bool isEnum() const {
    return record.getValueAsBit("isEnum");
  }
};
#endif
