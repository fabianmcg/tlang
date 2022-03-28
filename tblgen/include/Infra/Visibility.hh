#ifndef __INFRA_VISIBILITY_HH__
#define __INFRA_VISIBILITY_HH__

#include "Common.hh"

struct Visibility {
  static constexpr std::string_view kind = "Visibility";
  static constexpr std::string_view kinds[] = { "Public", "Protected", "Private" };
  typedef enum {
    None = -1,
    Public,
    Protected,
    Private
  } visibility_kind;
  llvm::Record &record;
  Visibility(llvm::Record &record) :
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
  inline bool isPublic() const {
    return record.getName().equals(kinds[0]);
  }
  inline bool isProtected() const {
    return record.getName().equals(kinds[1]);
  }
  inline bool isPrivate() const {
    return record.getName().equals(kinds[2]);
  }
  inline visibility_kind visibility() const {
    if (isPublic())
      return Public;
    else if (isProtected())
      return Protected;
    else if (isPrivate())
      return Private;
    return None;
  }
  static inline void emit(llvm::raw_ostream &ost, visibility_kind visibility) {
    switch (visibility) {
    case Public:
      ost << "public:";
      break;
    case Protected:
      ost << "protected:";
      break;
    case Private:
      ost << "private:";
      break;
    default:
      break;
    }
  }
  inline void emit(llvm::raw_ostream &ost) const {
    emit(ost, visibility());
  }
};
#endif
