#ifndef __INFRA_CODE_SECTION_HH__
#define __INFRA_CODE_SECTION_HH__

#include "Visibility.hh"

struct CodeSectionLocation {
  static constexpr std::string_view kind = "CodeSectionLocation";
  static constexpr std::string_view kinds[] = { "Header", "Source" };
  typedef enum {
    Header,
    Source
  } location_kind;
  llvm::Record &record;
  CodeSectionLocation(llvm::Record &record) :
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
  inline bool isHeader() const {
    return record.getName().equals(kinds[0]);
  }
  inline bool isSource() const {
    return record.getName().equals(kinds[1]);
  }
  inline location_kind location() const {
    if (isHeader())
      return Header;
    return Source;
  }
};
struct CodeSection {
  static constexpr std::string_view kind = "CodeSection";
  llvm::Record &record;
  CodeSection(llvm::Record &record) :
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
  inline CodeSectionLocation location() const {
    return CodeSectionLocation(*record.getValueAsDef("location"));
  }
  inline llvm::StringRef source() const {
    return record.getValueAsString("source");
  }
};
struct ClassSection: public CodeSection {
  using CodeSection::CodeSection;
  static constexpr std::string_view kind = "ClassSection";
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
  inline Visibility visibility() const {
    return Visibility(*record.getValueAsDef("visibility"));
  }
};
struct PrologueSection: public CodeSection {
  using CodeSection::CodeSection;
  static constexpr std::string_view kind = "Prologue";
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
  inline bool insideNamespace() const {
    return record.getValueAsBit("insideNamespace");
  }
};
struct EpilogueSection: public CodeSection {
  using CodeSection::CodeSection;
  static constexpr std::string_view kind = "Epilogue";
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
  inline bool insideNamespace() const {
    return record.getValueAsBit("insideNamespace");
  }
};

#endif
