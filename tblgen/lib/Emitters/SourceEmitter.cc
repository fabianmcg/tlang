#include <filesystem>
#include <ASTNode.hh>
#include <TGUtility.hh>
#include <CXXUtility.hh>
#include <Emitters/NodesEmitter.hh>

namespace {
struct SourceGen {
private:
  ASTNode &node;
  llvm::raw_ostream &ost;
  Visibility::visibility_kind kind = Visibility::None;
private:
  void emitConstructor(bool withKind, bool isConst) {
    ost << CXXFunction::constructor(CXXFunction::def, node, withKind ? "AbstractNode::set(kind);\n" : "", withKind, isConst) << "\n";
  }
  void emitConstructors() {
    auto &parents = node.parents();
    auto &children = node.children();
    auto &variables = node.members();
    emitConstructor(true, false);
    if (variables.size())
      emitConstructor(true, true);
    emitConstructor(false, false);
    if (variables.size())
      emitConstructor(false, true);
    if (!node.defaultDestructor().isUserDefined() && node.abstract())
      ost << node->getName() << "::~" << node->getName() << "() = default;\n\n";
    ost
        << CXXFunction::function(CXXFunction::def, node.name() + "::" + std::string(C::clasoff_v), CXXType("bool"),
            std::array<CXXVariable, 1> { CXXVariable { CXXType { std::string(C::kind_v) }, "kind" } },
            frmt("return ASTTraits<{}>::is(kind);", node.name())) << "\n";
  }
  void emitChildrenMethods() {
    auto &children = node.children();
    if (!children.empty()) {
      for (auto &child : children) {
        auto kind = child.childKind();
        std::string hasBody = " { return children.template has<" + capitalize(child.name.str()) + "Offset>(); }\n\n";
        std::string body = " { return children.template get<" + capitalize(child.name.str()) + "Offset>(); }\n\n";
        if (kind.isDynamic()) {
          ost << "bool " << node->getName() << "::has" << capitalize(child.name.str()) << "() const" << hasBody;
          ost << child.cxxType() << "& " << node->getName() << "::get" << capitalize(child.name.str()) << "()" << body;
          ost << "const " << child.cxxType() << " " << node->getName() << "::get" << capitalize(child.name.str()) << "() const" << body;
        } else if (kind.isStatic()) {
          ost << child.cxxType() << "& " << node->getName() << "::get" << capitalize(child.name.str()) << "()" << body;
          ost << "const " << child.cxxType() << "& " << node->getName() << "::get" << capitalize(child.name.str()) << "() const" << body;
        } else if (kind.isDynamicList()) {
          ost << child.cxxType() << "& " << node->getName() << "::get" << capitalize(child.name.str()) << "()" << body;
          ost << "const " << child.cxxType() << "& " << node->getName() << "::get" << capitalize(child.name.str()) << "() const" << body;
        } else if (kind.isStaticList()) {
          ost << child.cxxType() << "& " << node->getName() << "::get" << capitalize(child.name.str()) << "()" << body;
          ost << "const " << child.cxxType() << "& " << node->getName() << "::get" << capitalize(child.name.str()) << "() const" << body;
        }
      }
    }
  }
  void emitMemberMethods() {
    auto &variables = node.members();
    if (variables.empty())
      return;
    for (auto &var : variables) {
      auto type = strip(var.type().str());
      if (type == "bool")
        ost << "bool " << node->getName() << "::is" << capitalize(var.name.str()) << "() const { return " << var.name.str() << "; }\n\n";
      if (var.isEnum()) {
        auto type = strip(var.type().str());
        if (auto recVal = node->getValue(type))
          if (auto rec = llvm::dyn_cast<llvm::DefInit>(recVal->getValue()))
            if (Enum::is(rec->getDef())) {
              ost << node->getName() << "::" << var.type() << "& " << node->getName() << "::get" << capitalize(var.name.str())
                  << "() { return " << var.name.str() << "; }\n\n";
              ost << "const " << node->getName() << "::" << var.type() << "& " << node->getName() << "::get" << capitalize(var.name.str())
                  << "() const { return " << var.name.str() << "; }\n\n";
              for (auto er : Enum { recVal->getName(), *rec->getDef() }.members()) {
                EnumMember member { *er };
                ost << "bool " << node->getName() << "::is" << capitalize(member.identifier().str()) << "() const { return "
                    << var.name.str() << " == ";
                Enum e { recVal->getName(), *rec->getDef() };
                if (e.isClass())
                  ost << e.name << "::";
                ost << member.identifier() << "; }\n\n";
              }
            }
        continue;
      }
      ost << var.type() << "& " << node->getName() << "::get" << capitalize(var.name.str()) << "() { return " << var.name.str()
          << "; }\n\n";
      ost << "const " << var.type() << "& " << node->getName() << "::get" << capitalize(var.name.str()) << "() const { return "
          << var.name.str() << "; }\n\n";
    }
  }
  void emitClassSections() {
    auto &sections = node.sections();
    for (auto &section : sections) {
      if (section.source().empty() || !section.location().isSource())
        continue;
      ost << section.source() << "\n\n";
    }
  }
public:
  SourceGen(ASTNode &node, llvm::raw_ostream &ost) :
      node(node), ost(ost) {
  }
  void emit() {
    ost << "\n\n";
    centeredComment(ost, "BEGIN CLASS " + node->getName());
    ost << "\n";
    ost << "namespace " << C::namespace_v << " {\n";
    emitConstructors();
    emitChildrenMethods();
    emitMemberMethods();
    emitClassSections();
    centeredComment(ost, "END CLASS " + node->getName());
    ost << "}\n";
  }
};
}

void NodesEmitter::emitSource(llvm::raw_ostream &ost) {
  frmt("");
  std::filesystem::path path = std::filesystem::path(records.getInputFilename());
  std::string fn = path.stem().string();
  auto nodes = sortDefinitions(records, "", true);
  include(ost, "AST/" + fn + ".hh", false);
  include(ost, "AST/Traits.hh", false);
  for (auto record : nodes)
    if (!record->isAnonymous()) {
      if (AbstractNode::is(record)) {
        ASTNode node { *record };
        SourceGen { node, ost }.emit();
      } else if (CodeSection::is(record)) {
        CodeSection section { *record };
        if (section.location().isSource() && section.source().size())
          ost << "\n" << section.source() << "\n";
      }
    }
}
