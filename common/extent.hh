#ifndef __EXTENT_HH__
#define __EXTENT_HH__

#include <iostream>
#include <string>
#include <utility>

namespace ttc {
struct location {
  int line { -1 };
  int column { -1 };
  location() = default;
  ~location() = default;
  location(location&&) = default;
  location(const location&) = default;
  location& operator=(location&&) = default;
  location& operator=(const location&) = default;
  location(int l, int c) :
      line(l), column(c) {
  }
  bool valid() const {
    return line > 0 && column > 0;
  }
  void invalidate() {
    line = -1;
    column = -1;
  }
  bool operator<(const location &loc) const {
    return line < loc.line || (line <= loc.line && column < loc.column);
  }
  bool operator<=(const location &loc) const {
    return line < loc.line || (line <= loc.line && column <= loc.column);
  }
  bool operator>(const location &loc) const {
    return !(*this <= loc);
  }
  bool operator>=(const location &loc) const {
    return !(*this < loc);
  }
  bool operator==(const location &loc) const {
    return line == loc.line && column == loc.column;
  }
  std::string to_string() const {
    return "[" + std::to_string(line) + ":" + std::to_string(column) + "]";
  }
  bool compatible(const location &end) const {
    return valid() && end.valid() && (*this) <= end;
  }
  std::ostream& print(std::ostream &ost) const {
    ost << to_string();
    return ost;
  }
};
inline std::ostream& operator<<(std::ostream &ost, const location &loc) {
  return loc.print(ost);
}
using offsets_t= std::pair<int64_t, int64_t>;
struct Extent {
  location begin { };
  location end { };
  Extent() = default;
  ~Extent() = default;
  Extent(Extent&&) = default;
  Extent(const Extent&) = default;
  Extent& operator=(Extent&&) = default;
  Extent& operator=(const Extent&) = default;
  Extent(const location &begin, const location &end) :
      begin(begin), end(end) {
  }
  bool valid() const {
    return begin.compatible(end);
  }
  bool operator<(const Extent &ext) const {
    return begin < ext.begin || (begin <= ext.begin && end < ext.end);
  }
  int compare(const Extent &ext) const {
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
  bool inside(const Extent &ext) const {
    int c = compare(ext);
    return c == -1 || c == 1;
  }
  std::string to_string() const {
    return "[" + begin.to_string() + " - " + end.to_string() + "]";
  }
};
}
#endif
