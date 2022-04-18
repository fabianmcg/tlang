#ifndef SUPPORT_FORMAT_HH
#define SUPPORT_FORMAT_HH

#include <fmt/core.h>
#include <fmt/color.h>

namespace tlang {
template <typename ...Args>
std::string frmt(const std::string &str, Args &&...args) {
  return fmt::format(fmt::runtime(str), std::forward<Args>(args)...);
}

template <typename ...Args>
std::string frmt(const fmt::text_style &style, const std::string &str, Args &&...args) {
  return fmt::format(style, str, std::forward<Args>(args)...);
}

template <typename OStream, typename ...Args, std::enable_if_t<!std::is_same_v<std::decay_t<OStream>, std::string>, int> = 0>
OStream& frmt(OStream &ost, const fmt::text_style &style, const std::string &str, Args &&...args) {
  auto fs = frmt(style, str, std::forward<Args>(args)...);
  if (fs.size())
    ost << fs;
  return ost;
}

template <typename ...Args>
void print(const std::string &str, Args &&...args) {
  fmt::print(fmt::runtime(str), std::forward<Args>(args)...);
}

template <typename ...Args>
void print(const fmt::text_style &style, const std::string &str, Args &&...args) {
  fmt::print(style, fmt::runtime(str), std::forward<Args>(args)...);
}

template <typename ...Args>
void print(std::ostream &ost, const std::string &str, Args &&...args) {
  fmt::print(ost, fmt::runtime(str), std::forward<Args>(args)...);
}

template <typename ...Args>
void print(std::ostream &ost, const fmt::text_style &style, const std::string &str, Args &&...args) {
  frmt(ost, style, str, std::forward<Args>(args)...);
}
}

#endif
