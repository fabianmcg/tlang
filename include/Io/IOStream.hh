#ifndef __IO_IOSTREAM_HH__
#define __IO_IOSTREAM_HH__

#include <cstdint>
#include <iostream>

namespace tlang {
struct Color {
  static constexpr char Default_code[] = "\033[0m";
  enum ColorKind {
    Background,
    bg = Background,
    Foreground,
    fg = Foreground
  };
  const std::string code = Default_code;
  Color() = default;
  ~Color() = default;
  Color(Color&&) = default;
  Color(const Color&) = default;
  Color(const std::string &code) :
      code(code) {
  }
  Color& operator=(Color&&) = default;
  Color& operator=(const Color&) = default;
  static Color Default() {
    return Color { Default_code };
  }
  static Color Black() {
    return Color { "\033[30m" };
  }
  static Color Red() {
    return Color { "\033[31m" };
  }
  static Color Green() {
    return Color { "\033[32m" };
  }
  static Color Yellow() {
    return Color { "\033[33m" };
  }
  static Color Blue() {
    return Color { "\033[34m" };
  }
  static Color Magenta() {
    return Color { "\033[35m" };
  }
  static Color Cyan() {
    return Color { "\033[36m" };
  }
  static Color White() {
    return Color { "\033[37m" };
  }
  static Color Orange(ColorKind kind = fg) {
    return term_8bit(208, kind);
  }
  static Color AquaGreen(ColorKind kind = fg) {
    return term_8bit(84, kind);
  }
  static Color DarkGreen(ColorKind kind = fg) {
    return term_8bit(2, kind);
  }
  static Color RGB(uint8_t R, uint8_t G, uint8_t B, ColorKind kind = fg) {
    if (kind == fg)
      return Color { "\033[38;2;" + std::to_string(R) + ";" + std::to_string(G) + ";" + std::to_string(B) + "m" };
    else
      return Color { "\033[48;2;" + std::to_string(R) + ";" + std::to_string(G) + ";" + std::to_string(B) + "m" };
  }
  static Color term_8bit(uint8_t C, ColorKind kind = fg) {
    if (kind == fg)
      return Color { "\033[38;5;" + std::to_string(C) + "m" };
    else
      return Color { "\033[48;5;" + std::to_string(C) + "m" };
  }
  bool is_default() const {
    return code == Default_code;
  }
  bool operator==(const Color &color) const {
    return code == color.code;
  }
};
inline std::ostream& operator<<(std::ostream &stream, const Color &val) {
  stream << val.code;
  return stream;
}
}
#endif
