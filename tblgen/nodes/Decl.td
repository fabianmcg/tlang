#ifndef DECL_TD
#define DECL_TD
include "ASTNode.td"

def DeclHeader : CodeSection<Header, [{
#include <AST/Type.hh>
#include <AST/DeclContext.hh>
}]>;

class DeclContext;
class NoVisitDeclContext;
class VariableContext;

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

def ModuleDecl : AbstractNode<NamedDecl, /* Abstract = */ 0, /* Implicit = */ 0>, DeclContext {
}

def ExternFunctionDecl : AbstractNode<NamedDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child returnType = Child<Static, [{QualType}]>;
  Child parameters = Child<StaticList, [{QualType}]>;
}

def FunctorDecl : AbstractNode<NamedDecl, /* Abstract = */ 0, /* Implicit = */ 0>, VariableContext {
  Variable complete = Var<[{bool}]>;
  Child returnType = Child<Static, [{QualType}]>;
  Child parameters = Child<DynamicList, [{ParameterDecl}]>;
  Child body = Child<Dynamic, [{CompoundStmt}]>;
  ClassSection header = ClassSection<Header, Public, [{
    template <typename... Args>
    FunctorDecl(NamedDecl &&nameddecl, VariableContext &&declcontext, Args &&...args)
        : NamedDecl(std::move(nameddecl)), VariableContext(std::move(declcontext)), complete(false),
          children(std::forward<Args>(args)...) {
      addArgs(getParameters());
    }
    void addArgs(const List<ParameterDecl*>& params);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    void FunctorDecl::addArgs(const List<ParameterDecl*>& params) {
      for (auto &arg : params)
        add(arg);
    }
  }]>;
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
  ClassSection header = ClassSection<Header, Public, [{
    StructDecl(const Identifier &identifier, DeclContext &&context) : TagDecl(TypeDecl(identifier), std::move(context)) {
    }
  }]>;
}

def MethodDecl : AbstractNode<FunctorDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def VariableDecl : AbstractNode<NamedDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child type = Child<Static, [{QualType}]>;
  Child init = Child<Dynamic, [{Expr}]>;
}

def ParameterDecl : AbstractNode<VariableDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def MemberDecl : AbstractNode<VariableDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def EnumDecl : AbstractNode<TagDecl, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child type = Child<Static, [{QualType}]>;
  Child members = Child<DynamicList, [{EnumMemberDecl}]>;
  ClassSection header = ClassSection<Header, Public, [{
    EnumDecl(const Identifier &identifier, QualType &&type, std::vector<EnumMemberDecl *> &&members)
        : TagDecl(TypeDecl(identifier), DeclContext()), children(std::move(type), std::move(members)) {
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
