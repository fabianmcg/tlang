#include <list>
#include "CXXUtility.hh"
#include <Utility.hh>
#include <Constants.hh>

std::string CXXType::qualified() const {
  return frmt("{}{}{}", (isConst() ? "const " : ""), type, (isReference() ? "&" : (isRValue() ? "&&" : "")));
}

const std::string& CXXType::unqualified() const {
  return type;
}

std::string CXXVariable::decl() const {
  return frmt("{} {}", type.qualified(), identifier);
}

std::string CXXVariable::forward() const {
  return frmt("std::forward<{}>({})", type.unqualified(), identifier);
}

std::string CXXVariable::asInit() const {
  if (type.isRValue())
    return forward();
  return identifier;
}

namespace {
template <typename List, std::enable_if_t<std::is_same_v<typename List::value_type, std::pair<std::string, CXXVariable>>, int> = 0>
static std::string fwdConstructor(CXXFunction::Kind kind, const std::string &identifier, const List &arguments,
    const std::string &body = "") {
  auto header = frmt("{}({})", identifier, join(arguments, [](auto &o, auto &e) {
    o << e.second.decl();
  }));
  if (CXXFunction::decl == kind)
    return header + ";\n";
  auto init = sjoin(arguments, [](auto &e) -> std::string {
    return e.first.empty() ? std::string { } : frmt("{}", e.second.asInit());
  });
  return frmt("{}: {}(kind, {}){{{}}}\n", header, identifier, init, body);
}
std::string makeConstructor(CXXFunction::Kind kind, ASTNode &node, const std::string &body, bool withKind, bool isConst, bool ignoreBase) {
  auto &parents = node.parents();
  auto &children = node.children();
  auto &variables = node.members();
  auto argQuals = isConst ? CXXType::ConstReference : CXXType::RValue;
  size_t i = 0, n = parents.size() + children.size() + variables.size();
  std::list<std::pair<std::string, CXXVariable>> elements;
  if (withKind)
    elements.push_back( { std::string(), CXXVariable(CXXType(std::string(C::kind_v)), "kind") });
  for (auto &parent : parents) {
    if (ignoreBase && i == 0)
      continue;
    elements.push_back( { name(parent), CXXVariable(CXXType(name(parent), CXXType::RValue), frmt("p{}", i++)) });
  }
  i = 0;
  for (auto &var : variables)
    elements.push_back( { var.name.str(), CXXVariable(CXXType(var.type().str(), argQuals), frmt("{}{}", var.name.str(), i++)) });
  i = 0;
  for (auto &var : children)
    elements.push_back( { var.name.str(), CXXVariable(CXXType(var.cxxType(), argQuals), frmt("{}{}", var.name.str(), i++)) });
  if (withKind)
    return CXXFunction::constructor(kind, node.name(), elements, body);
  return fwdConstructor(kind, node.name(), elements, body);
}
}

std::string CXXFunction::constructor(Kind kind, ASTNode &node, const std::string &body, bool withKind, bool isConst) {
  auto tmp = makeConstructor(kind, node, body, withKind, isConst, false);
  if (withKind)
    return tmp;
  auto &parents = node.parents();
  auto &children = node.children();
  auto &variables = node.members();
  size_t n = parents.size() + children.size() + variables.size();
  bool ignoreBase = node.hasBase() && node.base().abstract() && (n > 1);
  if (ignoreBase)
    tmp += makeConstructor(kind, node, body, withKind, isConst, true);
  return tmp;
}
