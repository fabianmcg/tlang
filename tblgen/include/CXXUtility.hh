#ifndef __CXX_UTILITY_HH__
#define __CXX_UTILITY_HH__

#include "ASTNode.hh"
#include "Utility.hh"

class CXXType {
public:
  typedef enum {
    None = 0,
    Const = 1,
    Reference = 2,
    RValue = 4,
    ConstReference = Const | Reference
  } Qualifier;
  CXXType(const std::string &type, Qualifier qual = None) :
      type(type), qual(qual) {
  }
  bool isPlain() const {
    return qual == None;
  }
  bool isConst() const {
    return (qual & Const) == Const;
  }
  bool isReference() const {
    return (qual & Reference) == Reference;
  }
  bool isConstReference() const {
    return isConst() && isReference();
  }
  bool isRValue() const {
    return (qual & RValue) == RValue;
  }
  bool isConstRValue() const {
    return isConst() && isRValue();
  }
  std::string qualified() const;
  const std::string& unqualified() const;
private:
  std::string type { };
  Qualifier qual { };
};

struct CXXVariable {
  CXXVariable(const CXXType &type, const std::string &identifier) :
      type(type), identifier(identifier) {
  }
  std::string decl() const;
  std::string forward() const;
  std::string asInit() const;
  CXXType type;
  std::string identifier { };
};

class CXXFunction {
public:
  typedef enum {
    decl,
    def
  } Kind;
  template <typename List, std::enable_if_t<std::is_same_v<typename List::value_type, std::pair<std::string, CXXVariable>>, int> = 0>
  static std::string constructor(Kind kind, const std::string &identifier, const List &arguments, const std::string &body = "") {
    auto header = format("{0}({1})", identifier, join(arguments, [](auto &o, auto &e) {
      o << e.second.decl();
    }));
    if (decl == kind)
      return header + ";\n";
    auto init = sjoin(arguments, [](auto &e) -> std::string {
      return e.first.empty() || e.first == "children" ? std::string { } : format("{0}({1})", e.first, e.second.asInit());
    });
    auto tmp = sjoin(arguments, [](auto &e) -> std::string {
      return e.first != "children" ? std::string { } : e.second.asInit();
    });
    if (tmp.size())
        tmp = format("children({0})", tmp);
    init = init + (init.size() && tmp.size() ? ", " : "" ) + tmp;
    return format("{0}::{1}{2}{{{3}}\n", identifier, header, init.empty() ? std::string { } : ": " + init, body);
  }
  static std::string constructor(Kind kind, ASTNode &node, const std::string &body, bool withKind, bool isConst);
  template <typename List, std::enable_if_t<std::is_same_v<typename List::value_type, CXXVariable>, int> = 0>
  static std::string function(Kind kind, const std::string &identifier, const CXXType &returnType, const List &arguments,
      const std::string &body = "", const std::string &preffix = "") {
    auto header = format("{0}{1} {2}({3})", preffix, returnType.qualified(), identifier, join(arguments, [](auto &o, auto &e) {
      o << e.decl();
    }));
    if (decl == kind)
      return header + ";\n";
    return format("{0}{{{1}}\n", header, body);
  }
};

#endif
