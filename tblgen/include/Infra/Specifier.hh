#ifndef __INFRA_SPECIFIER_HH__
#define __INFRA_SPECIFIER_HH__

#include "Common.hh"

struct DeclSpecifier {
  static constexpr std::string_view kind = "DeclSpecifier";
  static constexpr std::string_view kinds[] = { "Default", "Deleted", "UserDefined" };
  llvm::Record &record;
  DeclSpecifier(llvm::Record &record) :
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
  inline bool isDefault() const {
    return record.getName().equals(kinds[0]);
  }
  inline bool isDeleted() const {
    return record.getName().equals(kinds[1]);
  }
  inline bool isUserDefined() const {
    return record.getName().equals(kinds[2]);
  }
};
#endif
