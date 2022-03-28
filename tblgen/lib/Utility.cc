#include <Utility.hh>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/Format.h>
#include <llvm/Support/raw_ostream.h>

void formatFile(const std::filesystem::path &path) {
  if (std::filesystem::exists(path)) {
    std::string command = "clang-format-13 -i " + path.string()
        + " -style=\"{BasedOnStyle: llvm, ColumnLimit: 120, AllowShortFunctionsOnASingleLine: None, BreakConstructorInitializers: AfterColon}\"";
    int r = std::system(command.c_str());
  }
}

std::string capitalize(const std::string &str) {
  if (str.size()) {
    std::string cpy = str;
    cpy[0] = std::toupper(cpy[0]);
    return cpy;
  }
  return "";
}

std::string toupper(const std::string &str) {
  if (str.size()) {
    std::string cpy = str;
    for (size_t k = 0; k < cpy.size(); ++k)
      cpy[k] = std::toupper(cpy[k]);
    return cpy;
  }
  return "";
}

std::string strip(const std::string &str) {
  auto it = str.begin();
  auto rit = str.rbegin();
  while ((it != str.end()) && std::isspace(*it))
    it++;
  while ((rit != str.rend()) && std::isspace(*rit))
    rit++;
  return std::string(it, rit.base());
}

void centeredComment(llvm::raw_ostream &ost, const llvm::Twine &comment, bool newLine, size_t MAX_LINE_LEN) {
  ost << "/***";
  ost << llvm::center_justify(comment.str(), MAX_LINE_LEN);
  ost << "***/";
  if (newLine)
    ost << "\n";
}

void defineGuards(llvm::raw_ostream &ost, const std::string &name, bool begin) {
  if (begin)
    ost << "#ifndef " << name << "\n#define " << name << "\n";
  else
    ost << "\n#endif\n";
}

void include(llvm::raw_ostream &ost, const std::string &name, bool sys) {
  ost << "#include " << (sys ? "<" : "\"") << strip(name) << (sys ? ">" : "\"") << "\n";
}
