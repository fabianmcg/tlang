#ifndef __INFRA_ABSTRACT_NODE_HH__
#define __INFRA_ABSTRACT_NODE_HH__

#include "Infra/Common.hh"
#include "Infra/Children.hh"
#include "Infra/CodeSection.hh"
#include "Infra/Enum.hh"
#include "Infra/Variable.hh"
#include "Infra/Specifier.hh"

struct AbstractNode {
  static constexpr std::string_view kind = "AbstractNode";
  llvm::Record &record;
  AbstractNode(llvm::Record &record) :
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
  llvm::Record& operator*() {
    return record;
  }
  llvm::Record* operator->() {
    return &record;
  }
  inline bool is() const {
    return is(&record);
  }
  inline bool hasBase() const {
    return !record.isValueUnset("base");
  }
  inline AbstractNode base() const {
    return AbstractNode(*record.getValueAsDef("base"));
  }
  inline std::string name() const {
    return record.getName().str();
  }
  inline bool abstract() const {
    return record.getValueAsBit("abstract");
  }
  inline bool implicit() const {
    return record.getValueAsBit("implicit");
  }
  inline bool top() const {
    return record.getValueAsBit("top");
  }
  inline DeclSpecifier copyable() const {
    return DeclSpecifier { *record.getValueAsDef("copyable") };
  }
  inline DeclSpecifier moveable() const {
    return DeclSpecifier { *record.getValueAsDef("moveable") };
  }
  inline DeclSpecifier defaultDestructor() const {
    return DeclSpecifier { *record.getValueAsDef("defaultDestructor") };
  }
  inline DeclSpecifier defaultConstructor() const {
    return DeclSpecifier { *record.getValueAsDef("defaultConstructor") };
  }
  inline bool visit() const {
    return record.getValueAsBit("visit");
  }
  inline bool implement() const {
    return record.getValueAsBit("implement");
  }
  inline bool emittable() const {
    return record.getValueAsBit("emittable");
  }
  inline bool unsetIrType() const {
    return record.isValueUnset("irType");
  }
  inline llvm::StringRef irType() const {
    return record.getValueAsString("irType");
  }
};
#endif
