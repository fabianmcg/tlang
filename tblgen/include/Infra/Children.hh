#ifndef __INFRA_CHILDREN_HH__
#define __INFRA_CHILDREN_HH__

#include "Common.hh"
#include <Utility.hh>

struct ChildKind {
  static constexpr std::string_view kind = "ChildKind";
  static constexpr std::string_view kinds[] = { "Dynamic", "Static", "DynamicList", "StaticList" };
  llvm::Record &record;
  ChildKind(llvm::Record &record) :
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
  inline bool isDynamic() const {
    return record.getName().equals(kinds[0]);
  }
  inline bool isStatic() const {
    return record.getName().equals(kinds[1]);
  }
  inline bool isDynamicList() const {
    return record.getName().equals(kinds[2]);
  }
  inline bool isStaticList() const {
    return record.getName().equals(kinds[3]);
  }
  std::string kindStr() const {
    if (isDynamic())
      return "ChildKind::Dynamic";
    else if (isStatic())
      return "ChildKind::Static";
    else if (isDynamicList())
      return "ChildKind::DynamicList";
    else if (isStaticList())
      return "ChildKind::StaticList";
    return "";
  }
};
struct Child {
  static constexpr std::string_view kind = "Child";
  llvm::StringRef name;
  llvm::Record &record;
  Child(llvm::StringRef name, llvm::Record &record) :
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
  inline ChildKind childKind() const {
    return ChildKind(*record.getValueAsDef("kind"));
  }
  inline llvm::StringRef type() const {
    return record.getValueAsString("type");
  }
  inline bool visit() const {
    return record.getValueAsBit("visit");
  }
  std::string cxxType() const {
    auto k = childKind();
    if (k.isDynamic())
      return (type() + "*").str();
    else if (k.isStatic())
      return type().str();
    else if (k.isDynamicList())
      return ("List<" + type() + "*>").str();
    else if (k.isStaticList())
      return ("List<" + type() + ">").str();
    return "";
  }
  std::string typeStr() const {
    return ("Child<" + childKind().kindStr() + ", " + type() + ", " + capitalize(name.str()) + "Offset, " + (visit() ? "true" : "false")
        + ">").str();
  }
};
#endif
