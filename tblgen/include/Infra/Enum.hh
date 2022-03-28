#ifndef __INFRA_ENUM_HH__
#define __INFRA_ENUM_HH__

#include "Common.hh"

struct EnumMember {
  static constexpr std::string_view kind = "EnumMember";
  llvm::Record &record;
  EnumMember(llvm::Record &record) :
      record(record) {
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
  inline llvm::StringRef identifier() const {
    return record.getValueAsString("identifier");
  }
  inline llvm::StringRef value() const {
    return record.getValueAsString("value");
  }
};
struct Enum {
  static constexpr std::string_view kind = "Enum";
  llvm::StringRef name;
  llvm::Record &record;
  Enum(llvm::StringRef name, llvm::Record &record) :
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
  inline std::vector<llvm::Record*> members() const {
    return record.getValueAsListOfDefs("members");
  }
  inline bool isClass() const {
    return record.getValueAsBit("isClass");
  }
};
#endif
