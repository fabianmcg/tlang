#ifndef __COMMON_SOURCE_RANGE_HH__
#define __COMMON_SOURCE_RANGE_HH__

#include <Common/Macros.hh>
#include <iostream>
#include <string>
#include <utility>

namespace __lang_np__ {
struct SourceLocation {
  int line { -1 };
  int column { -1 };
  SourceLocation() = default;
  ~SourceLocation() = default;
  SourceLocation(SourceLocation&&) = default;
  SourceLocation(const SourceLocation&) = default;
  SourceLocation& operator=(SourceLocation&&) = default;
  SourceLocation& operator=(const SourceLocation&) = default;
  SourceLocation(int l, int c) :
      line(l), column(c) {
  }
  bool valid() const {
    return line > 0 && column > 0;
  }
  void invalidate() {
    line = -1;
    column = -1;
  }
  bool operator<(const SourceLocation &loc) const {
    return line < loc.line || (line <= loc.line && column < loc.column);
  }
  bool operator<=(const SourceLocation &loc) const {
    return line < loc.line || (line <= loc.line && column <= loc.column);
  }
  bool operator>(const SourceLocation &loc) const {
    return !(*this <= loc);
  }
  bool operator>=(const SourceLocation &loc) const {
    return !(*this < loc);
  }
  bool operator==(const SourceLocation &loc) const {
    return line == loc.line && column == loc.column;
  }
  std::string to_string() const {
    return "[" + std::to_string(line) + ":" + std::to_string(column) + "]";
  }
  bool compatible(const SourceLocation &end) const {
    return valid() && end.valid() && (*this) <= end;
  }
  std::ostream& print(std::ostream &ost) const {
    ost << to_string();
    return ost;
  }
};
inline std::ostream& operator<<(std::ostream &ost, const SourceLocation &loc) {
  return loc.print(ost);
}
using offsets_t= std::pair<int64_t, int64_t>;
struct SourceRange {
  SourceLocation begin { };
  SourceLocation end { };
  SourceRange() = default;
  ~SourceRange() = default;
  SourceRange(SourceRange&&) = default;
  SourceRange(const SourceRange&) = default;
  SourceRange& operator=(SourceRange&&) = default;
  SourceRange& operator=(const SourceRange&) = default;
  SourceRange(const SourceLocation &begin, const SourceLocation &end) :
      begin(begin), end(end) {
  }
  bool valid() const {
    return begin.compatible(end);
  }
  bool operator<(const SourceRange &ext) const {
    return begin < ext.begin || (begin <= ext.begin && end < ext.end);
  }
  int compare(const SourceRange &ext) const {
    if (begin == ext.begin && ext.end == end)
      return 0;  // equal
    else if (begin <= ext.begin && ext.end <= end)
      return -1; // ext inside of this
    else if (ext.begin <= begin && end <= ext.end)
      return 1; // this inside of ext
    else if (begin < ext.begin && end < ext.end)
      return -2; // [x,y] comp [z, w], x < z < y < w
    else if (ext.begin < begin && ext.end < end)
      return 2;  // [x,y] comp [z, w], z < x < w < y
    return 3;
  }
  bool inside(const SourceRange &ext) const {
    int c = compare(ext);
    return c == -1 || c == 1;
  }
  std::string to_string() const {
    return "[" + begin.to_string() + " - " + end.to_string() + "]";
  }
};
} // namespace __lang_np__
#endif
