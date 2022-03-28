#include <map>
#include <llvm/TableGen/Record.h>
#include <llvm/TableGen/Error.h>
#include <llvm/Support/raw_ostream.h>
#include <TGUtility.hh>
#include <Infra/CodeSection.hh>
#include <Utility.hh>
#include <Constants.hh>

namespace {
inline bool isMainFile(llvm::Record *record) {
  if (!record)
    return false;
  auto locs = record->getLoc();
  if (!record->isAnonymous() && locs.size() && llvm::SrcMgr.getMainFileID() == llvm::SrcMgr.FindBufferContainingLoc(locs[0]))
    return true;
  return false;
}
}

std::vector<llvm::Record*> sortDefinitions(llvm::RecordKeeper &records, const std::string &ofKind, bool fromMain) {
  std::vector<llvm::Record*> definitions;
  auto &defs = records.getDefs();
  std::map<size_t, llvm::Record*> orderedDefs;
  for (auto &it : defs)
    if (llvm::Record *record = it.second.get())
      orderedDefs[record->getID()] = record;
  if (ofKind.empty()) {
    definitions.reserve(orderedDefs.size());
    for (auto& [key, record] : orderedDefs) {
      if (fromMain) {
        if (isMainFile(record))
          definitions.push_back(record);
      } else
        definitions.push_back(record);
    }
  } else {
    definitions.reserve(orderedDefs.size());
    for (auto& [key, record] : orderedDefs) {
      if (!record->isSubClassOf(ofKind))
        continue;
      if (fromMain) {
        if (isMainFile(record))
          definitions.push_back(record);
      } else
        definitions.push_back(record);
    }
  }
  return definitions;
}

void printPrologueSections(llvm::raw_ostream &ost, llvm::RecordKeeper &records, bool isHeader) {
  using section_t = PrologueSection;
  auto nodes = records.getAllDerivedDefinitions(section_t::kind);
  std::map<size_t, llvm::Record*> orderedSections;
  for (auto record : nodes)
    orderedSections[record->getID()] = record;
  for (auto& [key, record] : orderedSections) {
    if (!isMainFile(record))
      continue;
    section_t section { *record };
    if (section.location().isHeader() == isHeader) {
      auto inside = section.insideNamespace();
      if (inside)
        frmts(ost, "namespace {} {{\n", C::namespace_v);
      ost << section.source() << "\n";
      if (inside)
        ost << "}\n";
    }
  }
}

void printEpilogueSections(llvm::raw_ostream &ost, llvm::RecordKeeper &records, bool isHeader) {
  using section_t = EpilogueSection;
  auto nodes = records.getAllDerivedDefinitions(section_t::kind);
  std::map<size_t, llvm::Record*> orderedSections;
  for (auto record : nodes)
    orderedSections[record->getID()] = record;
  for (auto& [key, record] : orderedSections) {
    if (!isMainFile(record))
      continue;
    section_t section { *record };
    if (section.location().isHeader() == isHeader) {
      auto inside = section.insideNamespace();
      if (inside)
        frmts(ost, "namespace {} {{\n", C::namespace_v);
      ost << section.source() << "\n";
      if (inside)
        ost << "}\n";
    }
  }
}
