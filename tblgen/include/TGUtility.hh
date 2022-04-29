#ifndef __SORT_NODES_HH__
#define __SORT_NODES_HH__

#include <vector>
#include <string>

namespace llvm {
class RecordKeeper;
class Record;
class raw_ostream;
}

std::vector<llvm::Record*> sortDefinitions(llvm::RecordKeeper &records, const std::string &ofKind = "", bool fromMain = false);
void printPrologueSections(llvm::raw_ostream& ost, llvm::RecordKeeper &records, bool isHeader);
void printEpilogueSections(llvm::raw_ostream& ost, llvm::RecordKeeper &records, bool isHeader);

#endif
