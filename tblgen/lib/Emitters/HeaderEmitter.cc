#include <array>
#include <filesystem>
#include <ASTNode.hh>
#include <TGUtility.hh>
#include <CXXUtility.hh>
#include <Emitters/NodesEmitter.hh>

namespace {
struct HeaderGen {
private:
  ASTNode &node;
  llvm::raw_ostream &ost;
  Visibility::visibility_kind kind = Visibility::None;
private:
  void emitVisibility(Visibility::visibility_kind k) {
    if (k != kind) {
      kind = k;
      Visibility::emit(ost, kind);
      ost << "\n";
    }
  }
  void emitSuperClasses() {
    auto parents = node.parents();
    if (parents.size()) {
      ost << ": ";
      join(ost, parents, [](auto &ost, auto &element) {
        ost << "public " << element->getName();
      });
    }
  }
  void emitNodeInfo() {
    emitVisibility(Visibility::Public);
    auto &parents = node.parents();
    auto &children = node.children();
    centeredComment(ost, "Class Information");
    frmts(ost, "using {} = {};\n", C::base_type_v, node.base().name());
    frmts(ost, "using {} = {}<{}>;\n", C::parents_v, C::parent_class_v, join(parents, [](auto &ost, auto &element) {
      ost << name(element);
    }));
    frmts(ost, "static constexpr {0} kind = {0}::{1};\n", C::kind_v, node.name());
    if (children.empty()) {
      frmts(ost, "enum {} {{ {} }};", C::offsets_v, C::offset("End"));
      frmts(ost, "using {} = {}<>;\n", C::children_list_v, C::children_class_v);
    } else {
      frmts(ost, "enum {} {{ {}, {} }};", C::offsets_v, join(children, [](auto &ost, auto &element) {
        ost << C::offset(element.name.str());
      }), C::offset("End"));
      frmts(ost, "using {} = {}<{}>;\n", C::children_list_v, C::children_class_v, join(children, [](auto &ost, auto &element) {
        ost << element.typeStr();
        return true;
      }));
    }
    ost << "using AbstractNode::classof;\n";
  }
  void emitConstructor(bool withKind, bool isConst) {
    ost << CXXFunction::constructor(CXXFunction::decl, node, "", withKind, isConst);
  }
  void emitConstructors() {
    auto &parents = node.parents();
    auto &children = node.children();
    auto &variables = node.members();
    centeredComment(ost, "Constructors");
    emitVisibility(Visibility::Protected);
    frmts(ost, "{}(int) {{AbstractNode::set(kind);}}\n", node.name(), C::base_type_v);
    emitConstructor(true, false);
    if (variables.size())
      emitConstructor(true, true);
    if (!node.defaultConstructor().isUserDefined())
      emitVisibility(Visibility::Public);
    emitConstructor(false, false);
    if (variables.size())
      emitConstructor(false, true);
    emitVisibility(Visibility::Public);
    if (node.defaultConstructor().isDefault())
      ost << node->getName() << "():" << node->getName() << "(0) {}\n";
    else if (node.defaultConstructor().isDeleted())
      ost << node->getName() << "() = deleted;\n";
    if (!node.defaultDestructor().isUserDefined()) {
      ost << "virtual ~" << node->getName() << "()";
      if (node.abstract())
        ost << " = 0;\n";
      else
        ost << " = default;\n";
    }
    if (node.copyable().isDefault())
      ost << node->getName() << "(const " << node->getName() << "& other) = default;\n";
    else if (node.copyable().isDeleted())
      ost << node->getName() << "(const " << node->getName() << "& other) = delete;\n";
    if (node.moveable().isDefault())
      ost << node->getName() << "(" << node->getName() << "&& other) = default;\n";
    else if (node.moveable().isDeleted()) {
      emitVisibility(Visibility::Protected);
      ost << node->getName() << "(" << node->getName() << "&& other) = default;\n";
    }
    emitVisibility(Visibility::Public);
    ost << CXXFunction::function(CXXFunction::decl, std::string(C::clasoff_v), CXXType("bool"), std::array<CXXVariable, 1> { CXXVariable {
        CXXType { std::string(C::kind_v) }, "kind" } }, "", "static ");
  }
  void emitAssignmentOperators() {
    centeredComment(ost, "Assignment Operators");
    emitVisibility(Visibility::Public);
    if (node.copyable().isDefault())
      ost << node->getName() << "& operator=" << "(const " << node->getName() << "& other) = default;\n";
    else if (node.copyable().isDeleted())
      ost << node->getName() << "& operator=" << "(const " << node->getName() << "& other) = delete;\n";
    if (node.moveable().isDefault())
      ost << node->getName() << "& operator=(" << node->getName() << "&& other) = default;\n";
    else if (node.moveable().isDeleted()) {
      emitVisibility(Visibility::Protected);
      ost << node->getName() << "& operator=(" << node->getName() << "&& other) = default;\n";
    }
  }
  void emitEnum(Enum &e) {
    ost << (e.isClass() ? "enum class " : "typedef enum {");
    for (auto record : e.members()) {
      if (EnumMember::is(record)) {
        EnumMember member { *record };
        std::string init = strip(member.value().str());
        ost << member.identifier();
        if (init.size())
          ost << " = " << init;
        ost << ",";
      }
    }
    if (!e.isClass())
      ost << "} " << e.name << ";\n";
    else
      ost << "};\n";
  }
  void emitEnums() {
    auto &enums = node.enums();
    if (enums.empty())
      return;
    emitVisibility(Visibility::Public);
    centeredComment(ost, "Enumerations");
    for (auto &E : enums)
      emitEnum(E);
  }
  void emitChildrenMethods() {
    emitVisibility(Visibility::Public);
    centeredComment(ost, "Children Methods");
    auto &children = node.children();
    ost << "static constexpr bool hasChildren() { return children_list::size; };\n";
    if (!children.empty()) {
      ost << "inline children_list* operator->() { return &children; };\n";
      ost << "inline const children_list* operator->() const { return &children; };\n";
      ost << "inline children_list& operator*() { return children; };\n";
      ost << "inline const children_list& operator*() const { return children; };\n";
      for (auto &child : children) {
        auto kind = child.childKind();
        if (kind.isDynamic()) {
          ost << "bool has" << capitalize(child.name.str()) << "() const;\n";
          ost << child.cxxType() << "&" << "get" << capitalize(child.name.str()) << "();\n";
          ost << "const " << child.cxxType() << " get" << capitalize(child.name.str()) << "() const;\n";
        } else if (kind.isStatic()) {
          ost << child.cxxType() << "&" << " get" << capitalize(child.name.str()) << "();\n";
          ost << "const " << child.cxxType() << "&" << " get" << capitalize(child.name.str()) << "() const;\n";
        } else if (kind.isDynamicList()) {
          ost << child.cxxType() << "&" << " get" << capitalize(child.name.str()) << "();\n";
          ost << "const " << child.cxxType() << "&" << " get" << capitalize(child.name.str()) << "() const;\n";
        } else if (kind.isStaticList()) {
          ost << child.cxxType() << "&" << " get" << capitalize(child.name.str()) << "();\n";
          ost << "const " << child.cxxType() << "&" << " get" << capitalize(child.name.str()) << "() const;\n";
        }
      }
    }
  }
  void emitMemberMethods() {
    auto &variables = node.members();
    if (variables.empty())
      return;
    emitVisibility(Visibility::Public);
    centeredComment(ost, "Members Methods");
    for (auto &var : variables) {
      ost << var.type() << "&" << " get" << capitalize(var.name.str()) << "();\n";
      ost << "const " << var.type() << "&" << " get" << capitalize(var.name.str()) << "() const;\n";
      auto type = strip(var.type().str());
      if (type == "bool")
        ost << "bool" << " is" << capitalize(var.name.str()) << "() const;\n";
      if (var.isEnum()) {
        auto type = strip(var.type().str());
        if (auto recVal = node->getValue(type))
          if (auto rec = llvm::dyn_cast<llvm::DefInit>(recVal->getValue()))
            if (Enum::is(rec->getDef()))
              for (auto er : Enum { recVal->getName(), *rec->getDef() }.members()) {
                EnumMember member { *er };
                ost << "bool" << " is" << capitalize(member.identifier().str()) << "() const;\n";
              }
      }
    }
  }
  void emitClassSections() {
    auto &sections = node.sections();
    for (auto &section : sections) {
      if (section.source().empty() || !section.location().isHeader())
        continue;
      emitVisibility(section.visibility().visibility());
      ost << section.source() << "\n";
    }
  }
  void emitClassMembers() {
    if (node.children().empty() && node.members().empty())
      return;
    emitVisibility(Visibility::Protected);
    centeredComment(ost, "Class members");
    if (node.children().size())
      ost << "children_list children { };\n";
    auto &variables = node.members();
    for (auto &var : variables) {
      emitVisibility(var.visibility().visibility());
      ost << var.type() << " " << var.name;
      if (var.init().empty())
        ost << "{};\n";
      else
        ost << " = " << var.init() << ";\n";
    }
  }
public:
  HeaderGen(ASTNode &node, llvm::raw_ostream &ost) :
      node(node), ost(ost) {
  }
  void emit() {
    ost << "namespace " << C::namespace_v << " {\n";
    ost << "class " << node.record.getName();
    emitSuperClasses();
    ost << " {\n";
    emitNodeInfo();
    emitEnums();
    emitConstructors();
    emitAssignmentOperators();
    emitChildrenMethods();
    emitMemberMethods();
    emitClassSections();
    emitClassMembers();
    ost << "};\n";
    ost << "}\n";
  }
};
}

void NodesEmitter::emitHeader(llvm::raw_ostream &ost) {
  std::filesystem::path path = std::filesystem::path(records.getInputFilename());
  std::string fn = path.stem().string();
  llvm::emitSourceFileHeader("AST " + fn + " header", ost);
  auto nodes = sortDefinitions(records, "", true);
  defineGuards(ost, "AST_" + toupper(fn) + "_HH", true);
  include(ost, "ASTNode.hh", false);
  for (auto record : nodes)
    if (!record->isAnonymous()) {
      if (AbstractNode::is(record)) {
        ASTNode node { *record };
        HeaderGen { node, ost }.emit();
      } else if (CodeSection::is(record)) {
        CodeSection section { *record };
        if (section.location().isHeader() && section.source().size())
          ost << "\n" << section.source() << "\n";
      }
    }
  defineGuards(ost, "AST_" + toupper(fn) + "_HH", false);
}
