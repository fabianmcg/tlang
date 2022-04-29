#ifndef ADT_LITERAL_HH
#define ADT_LITERAL_HH

#include <cstdint>
#include <string>

namespace tlang {
class LiteralValue {
public:
  typedef enum {
    Bool,
    String,
    Int,
    Uint,
    Float
  } Kind;
  LiteralValue(Kind kind) :
      kind(kind) {
  }
  virtual ~LiteralValue() = default;
  virtual std::string to_string() const = 0;
  Kind getKind() const {
    return kind;
  }
  template <typename T>
  T& getAs() {
    return *static_cast<T*>(this);
  }
protected:
  Kind kind;
};

class BoolValue final: public LiteralValue {
public:
  BoolValue(bool value) :
      LiteralValue(Bool), literal(value) {
  }
  bool& getValue() {
    return literal;
  }
  bool getValue() const {
    return literal;
  }
  std::string to_string() const {
    return literal ? "true" : "false";
  }
private:
  bool literal { };
};

class StringValue final: public LiteralValue {
public:
  StringValue(const std::string &value) :
      LiteralValue(String), literal(value) {
  }
  std::string getValue() const {
    return literal;
  }
  std::string to_string() const {
    return literal;
  }
private:
  std::string literal { };
};

class IntValue final: public LiteralValue {
public:
  IntValue(int64_t value) :
      LiteralValue(Int), literal(value) {
  }
  int64_t& getValue() {
    return literal;
  }
  int64_t getValue() const {
    return literal;
  }
  std::string to_string() const {
    return std::to_string(literal);
  }
private:
  int64_t literal { };
};

class UIntValue final: public LiteralValue {
public:
  UIntValue(uint64_t value) :
      LiteralValue(Uint), literal(value) {
  }
  uint64_t& getValue() {
    return literal;
  }
  uint64_t getValue() const {
    return literal;
  }
  std::string to_string() const {
    return std::to_string(literal);
  }
private:
  uint64_t literal { };
};

class FloatValue final: public LiteralValue {
public:
  FloatValue(double value) :
      LiteralValue(Float), literal(value) {
  }
  double& getValue() {
    return literal;
  }
  double getValue() const {
    return literal;
  }
  std::string to_string() const {
    return std::to_string(literal);
  }
private:
  double literal { };
};
}
#endif
