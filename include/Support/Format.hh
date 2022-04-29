#ifndef SUPPORT_FORMAT_HH
#define SUPPORT_FORMAT_HH

//#include <fmt/core.h>
//#include <fmt/color.h>
#include <llvm/Support/FormatVariadic.h>
#include <iostream>

namespace tlang {
template <typename ...Args>
std::string frmt(const std::string &str, Args &&...args) {
  return llvm::formatv(str.c_str(), std::forward<Args>(args)...);
}

template <typename ...Args>
void print(const std::string &str, Args &&...args) {
  std::cout << frmt(str, std::forward<Args>(args)...);
}

template <typename ...Args>
void print(std::ostream &ost, const std::string &str, Args &&...args) {
  ost << frmt(str, std::forward<Args>(args)...);
}
}

#endif
