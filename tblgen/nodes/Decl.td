#ifndef DECL_TD
#define DECL_TD
include "ASTNode.td"

def DeclHeader : CodeSection<Header, [{
#include <AST/Type.hh>
#include <AST/DeclContext.hh>
#include <CodeGen/Backend.hh>
}]>;

class UniverseContext;
class UnitContext;
class DeclContext;
class NoVisitDeclContext;
class VariableContext;
class Backend;

def Decl : AbstractNode<ASTNode, /* Abstract = */ 1, /* Implicit = */ 0, /* Top = */ 1> {
}

def NamedDecl : AbstractNode<Decl, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable identifier = Var<[{Identifier}]>;
  ClassSection epilogue = ClassSection<Header, Public, [{
    virtual std::string usr() const {
      return identifier;
    }
  }]>;
}

def UniverseDecl : AbstractNode<Decl, /* Abstract = */ 0, /* Implicit = */ 0>, UniverseContext {
}

def UnitDecl : AbstractNode<NamedDecl, /* Abstract = */ 0, /* Implicit = */ 0>, UnitContext, Backend {
}

def ModuleDecl : AbstractNode<NamedDecl, /* Abstract = */ 0, /* Implicit = */ 0>, DeclContext {
}

def ValueDecl : AbstractNode<NamedDecl, /* Abstract = */ 1, /* Implicit = */ 0> {
  Child type = Child<Static, [{QualType}]>;
  ClassSection header = ClassSection<Header, Public, [{
    ValueDecl(const Identifier& identifier);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    ValueDecl::ValueDecl(const Identifier& identifier): ValueDecl(NamedDecl(identifier), QualType()) {}
  }]>;
}

def FunctorDecl : AbstractNode<ValueDecl, /* Abstract = */ 0, /* Implicit = */ 0>, VariableContext {
  Enum GenKind = Enum<[EM<"Generic", [{0}]>, EM<"Kernel", [{1}]>]>;
  Variable functionKind = EVar<[{GenKind}]>;
  Variable complete = Var<[{bool}]>;
  Child returnType = Child<Static, [{QualType}]>;
  Child parameters = Child<DynamicList, [{ParameterDecl}]>;
  Child body = Child<Dynamic, [{CompoundStmt}]>;
  ClassSection header = ClassSection<Header, Public, [{
    template <typename... Args>
    FunctorDecl(const Identifier& identifier, Args &&...args)
        : FunctorDecl(VariableContext(), Generic, false, std::forward<Args>(args)...) {
      addArgs(getParameters());
      getIdentifier() = identifier;
    }
    void addArgs(const List<ParameterDecl*>& params);
    FunctionType* setType(TypeContext& context);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    void FunctorDecl::addArgs(const List<ParameterDecl*>& params) {
      for (auto &arg : params)
        add(arg);
    }
    FunctionType* FunctorDecl::setType(TypeContext& context) {
      auto& parameters = getParameters();
      QualType rt = getReturnType();
      List<QualType> args(parameters.size());
      for (size_t i = 0; i < args.size(); ++i)
        args[i] = parameters[i]->getType();
      auto type = FunctionType::get(&context, std::move(rt), std::move(args));
      getType() = QualType(type);
      return type;
    }
  }]>;
  code irType = [{llvm::Function*}];
}

def ExternFunctionDecl : AbstractNode<FunctorDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def FunctionDecl : AbstractNode<FunctorDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def ImportDecl : AbstractNode<Decl, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable moduleName = Var<[{Identifier}]>;
  Variable module = Var<[{reference<ModuleDecl>}]>;
}

def UsingDecl : AbstractNode<NamedDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child type = Child<Static, [{QualType}]>;
  ClassSection header = ClassSection<Header, Public, [{
    UsingDecl(const Identifier &identifier, QualType &&type) : NamedDecl(identifier), children(std::move(type)) {
    }
  }]>;
}

def TypeDecl : AbstractNode<NamedDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    TypeDecl(const Identifier &identifier) : NamedDecl(identifier) {
    }
  }]>;
}

def TagDecl : AbstractNode<TypeDecl, /* Abstract = */ 0, /* Implicit = */ 0>, DeclContext {
}

def StructDecl : AbstractNode<TagDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable members = Var<[{List<MemberDecl*>}]>;
  ClassSection header = ClassSection<Header, Public, [{
    StructDecl(const Identifier &identifier, DeclContext &&context);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    StructDecl::StructDecl(const Identifier &identifier, DeclContext &&context) : StructDecl(TagDecl(TypeDecl(identifier), std::move(context)), List<MemberDecl*>{}) {
      auto &members = getMembers();
      for (auto symbol : *this) {
        if (auto md = dyn_cast<MemberDecl>(symbol)) {
          md->getIndex() = members.size();
          members.push_back(md);
        }
      }
    }
  }]>;
}

def MethodDecl : AbstractNode<FunctorDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def VariableDecl : AbstractNode<ValueDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
  Enum StorageKind = Enum<[EM<"Generic", [{0}]>, EM<"Static", [{1}]>, EM<"Extern", [{2}]>, EM<"Local", [{4}]>, EM<"Global", [{8}]>, EM<"Shared", [{16}]>]>;
  Variable storage = EVar<[{StorageKind}]>;
  Child init = Child<Dynamic, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    VariableDecl(const Identifier& identifier, QualType&& type, Expr* init = nullptr, StorageKind kind = Generic);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    VariableDecl::VariableDecl(const Identifier& identifier, QualType&& type, Expr* init, StorageKind kind): VariableDecl(kind, init) {
      getType() = std::move(type);
      getIdentifier() = identifier;
    }
  }]>;
}

def GlobalVarDecl : AbstractNode<VariableDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def ParameterDecl : AbstractNode<VariableDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def MemberDecl : AbstractNode<VariableDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable index = Var<[{int}]>;
}

def EnumDecl : AbstractNode<TagDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child type = Child<Static, [{QualType}]>;
  Child members = Child<DynamicList, [{EnumMemberDecl}]>;
  ClassSection header = ClassSection<Header, Public, [{
    EnumDecl(const Identifier &identifier, QualType &&type, std::vector<EnumMemberDecl *> &&members)
        : EnumDecl(TagDecl(TypeDecl(identifier), DeclContext()), std::move(type), std::move(members)) {
    }
  }]>;
}

def EnumMemberDecl : AbstractNode<NamedDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child value = Child<Dynamic, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    EnumMemberDecl(const Identifier &identifier, Expr *value = nullptr) : NamedDecl(identifier), children(value) {
    }
  }]>;
}

#endif
