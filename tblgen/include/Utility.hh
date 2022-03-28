#ifndef __UTILITY_HH__
#define __UTILITY_HH__

#include <filesystem>
#include <sstream>
#include <string>
#include <string_view>
#include <fmt/core.h>

void formatFile(const std::filesystem::path &path);

std::string capitalize(const std::string &str);

std::string toupper(const std::string &str);

std::string strip(const std::string &str);

namespace llvm {
class raw_ostream;
class Twine;
}
void centeredComment(llvm::raw_ostream &ost, const llvm::Twine &comment, bool newLine = true, size_t MAX_LINE_LEN = 80);

void defineGuards(llvm::raw_ostream &ost, const std::string &name, bool begin);

void include(llvm::raw_ostream &ost, const std::string &name, bool sys = false);

template <typename C, typename F>
std::string sjoin(C &&container, F &&printer, const std::string &sep = ", ") {
  std::ostringstream ost;
  if (container.size()) {
    size_t idx { };
    for (auto &element : container) {
      auto tmp = printer(element);
      if (tmp.empty())
        continue;
      if (idx++)
        ost << sep;
      ost << tmp;
    }
  }
  return ost.str();
}

template <typename O, typename C, typename F>
void join(O &ost, C &&container, F &&printer, const std::string &sep = ", ") {
  if (container.size()) {
    size_t idx { };
    for (auto &element : container) {
      if (idx++)
        ost << sep;
      printer(ost, element);
    }
  }
}

template <typename C, typename F>
std::string join(C &&container, F &&printer, const std::string &sep = ", ") {
  std::ostringstream ost;
  join(ost, std::forward<C>(container), std::forward<F>(printer), sep);
  return ost.str();
}

template <typename ...Args>
std::string sfrmt(const fmt::format_string<Args...> &str, Args &&...args) {
  return fmt::format(str, std::forward<Args>(args)...);
}

template <typename ...Args>
std::string frmt(const std::string &str, Args &&...args) {
  return fmt::format(fmt::runtime(str), std::forward<Args>(args)...);
}

template <typename O, typename ...Args>
O& sfrmts(O &ost, const fmt::format_string<Args...> &str, Args &&...args) {
  auto fs = sfrmt(str, std::forward<Args>(args)...);
  if (fs.size())
    ost << fs;
  return ost;
}

template <typename O, typename ...Args>
O& frmts(O &ost, const std::string &str, Args &&...args) {
  auto fs = frmt(str, std::forward<Args>(args)...);
  if (fs.size())
    ost << fs;
  return ost;
}

#endif
