#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Lang.db import LangDB
from Utility.enum import Enumeration
from Utility.type import EnumType, Type
from Utility.variable import variableDict as V
from Utility.struct import ClassMembers as Members, ClassParents as Parents, ClassTypes as Subtypes
from Lang.node import ChildrenList as Children, Dynamic, ManyDynamic, Static


def declareAttrs(addNode):
    addNode("Attr", base=True)
    addNode("AttrList")
    addNode("AttrWithLocation", loc=True)
    addNode("NamedAttr")
    addNode("TypeAttr")
    addNode("ExprAttr")
    addNode("LiteralAttr")
    addNode("DeclAttr")
    addNode("ContextAttr")
    addNode("PolicyAttr")


def defineAttrs(N, T, DT):
    with N.AttrList as node:
        node <<= Children(ManyDynamic(T.Attr, "attributes"))
    with N.AttrWithLocation as node:
        node <<= Parents(T.Attr)
        node <<= Children(Dynamic(T.Attr, "attr"))
    with N.NamedAttr as node:
        node <<= Parents(T.Attr)
        node <<= Members(V.V(T.Identifier, "identifier"))
        node <<= Children(Dynamic(T.Attr, "attr"))
    with N.TypeAttr as node:
        node <<= Parents(T.Attr)
        node <<= Children(Static(T.QualType, "type"))
    with N.ExprAttr as node:
        node <<= Parents(T.Attr)
        node <<= Children(Dynamic(T.Expr, "expr"))
    with N.LiteralAttr as node:
        node <<= Parents(T.ExprAttr)
    with N.DeclAttr as node:
        node <<= Parents(T.Attr)
        node <<= Children(Dynamic(T.Decl, "decl"))
    with N.ContextAttr as node:
        node <<= Parents(T.DeclAttr)
    with N.PolicyAttr as node:
        node <<= Parents(T.DeclAttr)


def declareTypes(addNode):
    addNode("Type", base=True)
    addNode("DeducedType")
    addNode("AutoType")
    addNode("BuiltinType")
    addNode("PtrType")
    addNode("SmartPtrType")
    addNode("ArrayType")
    addNode("DefinedType")
    addNode("TagType")
    addNode("StructType")
    addNode("EnumType")
    addNode("TypedefType")


def defineTypes(N, T, DT):
    with N.DeducedType as node:
        node <<= Parents(T.Type)
    with N.AutoType as node:
        node <<= Parents(T.DeducedType)
    with N.BuiltinType as node:
        node <<= Parents(T.Type)
        node <<= Subtypes(
            Enumeration.create("builtin_types", ["Int", "Float", "Complex", "String", "Void"]),
            Enumeration.create(
                "numeric_precision",
                ["Unk_np", "Int_8", "Int_16", "Int_32", "Int_64", "Float_16", "Float_32", "Float_64", "Float_128"],
            ),
            Enumeration.create("signed_kind", ["Unk_sign", "Unsigned", "Signed"]),
        )
        node <<= Members(
            V.V(EnumType("builtin_types"), "kind"),
            V.V(EnumType("numeric_precision"), "precision"),
            V.V(EnumType("signed_kind"), "signedness"),
        )
    with N.PtrType as node:
        node <<= Parents(T.Type)
        node <<= Children(Dynamic(T.Type, "underlying"))
    with N.SmartPtrType as node:
        node <<= Parents(T.PtrType)
    with N.ArrayType as node:
        node <<= Parents(T.Type)
        node <<= Members(V.V(T.Size, "size"))
        node <<= Children(Dynamic(T.Type, "underlying"))
    with N.DefinedType as node:
        node <<= Parents(T.Type)
        node <<= Members(
            V.V(T.Identifier, "identifier"),
            V.R(T.TagDecl, "decl"),
        )
    with N.TagType as node:
        node <<= Parents(T.DefinedType)
    with N.StructType as node:
        node <<= Parents(T.TagType)
    with N.EnumType as node:
        node <<= Parents(T.TagType)
    with N.TypedefType as node:
        node <<= Parents(T.DefinedType)
    with DT.QualType as node:
        node <<= Subtypes(
            Enumeration.create("cvr_qualifiers", [("No_quals", 0), ("Const", 1), ("Reference", 2)]),
            Enumeration.create("memory_qualifiers", [("Local", 1), ("Global", 2), ("Shared", 3), ("Constant", 8)]),
        )
        node <<= Members(
            V.UP(T.Type, "type"),
            V.V(EnumType("cvr_qualifiers", True), "qualifiers"),
            V.V(EnumType("memory_qualifiers", True), "mem_qualifiers"),
        )


def declareDecls(addNode):
    addNode("Decl", loc=True, base=True)
    addNode("NamedDecl")
    addNode("ModuleDecl")
    addNode("ImportDecl")
    addNode("UsingDecl")
    addNode("TypeDecl")
    addNode("ContextDecl")
    addNode("PolicyDecl")
    addNode("TypedefDecl")
    addNode("TagDecl")
    addNode("StructDecl")
    addNode("EnumDecl")
    addNode("FunctorDecl")
    addNode("FunctionDecl")
    addNode("MethodDecl")
    addNode("VarDecl")
    addNode("ParDecl")
    addNode("MemberDecl")
    addNode("EnumMemberDecl")


def defineDecls(N, T, DT):
    with N.Decl as node:
        node <<= Children(Dynamic(T.AttrList, "attributes"))
    with N.NamedDecl as node:
        node <<= Parents(T.Decl)
        node <<= Members(V.V(T.Identifier, "identifier"))
    with N.ModuleDecl as node:
        node <<= Parents(T.NamedDecl, T.DeclContext)
    with N.ImportDecl as node:
        node <<= Parents(T.Decl)
        node <<= Members(
            V.V(T.Identifier, "moduleName"),
            V.R(T.ModuleDecl, "module"),
        )
    with N.UsingDecl as node:
        node <<= Parents(T.Decl)
        node <<= Members(
            V.V(T.Identifier, "declName"),
            V.R(T.Decl, "decl"),
        )
    with N.TypeDecl as node:
        node <<= Parents(T.NamedDecl)
    with N.ContextDecl as node:
        node <<= Parents(T.TypeDecl, T.DeclContext)
    with N.PolicyDecl as node:
        node <<= Parents(T.TypeDecl, T.DeclContext)
    with N.VarDecl as node:
        node <<= Parents(T.NamedDecl)
        node <<= Children(
            Static(T.QualType, "type"),
            Dynamic(T.Expr, "init"),
        )
    with N.ParDecl as node:
        node <<= Parents(T.VarDecl)
    with N.MemberDecl as node:
        node <<= Parents(T.VarDecl)
    with N.TypedefDecl as node:
        node <<= Parents(T.TypeDecl)
        node <<= Children(Static(T.QualType, "type"))
    with N.TagDecl as node:
        node <<= Parents(T.TypeDecl, T.DeclContext)
    with N.StructDecl as node:
        node <<= Parents(T.TagDecl)
    with N.EnumDecl as node:
        node <<= Parents(T.TagDecl)
        node <<= Children(Static(T.QualType, "type"))
    with N.FunctorDecl as node:
        node <<= Parents(T.NamedDecl, T.DeclContext)
        node <<= Children(
            Dynamic(T.CompoundStmt, "body"),
        )
    with N.FunctionDecl as node:
        node <<= Parents(T.FunctorDecl)
    with N.MethodDecl as node:
        node <<= Parents(T.FunctorDecl)
    with N.EnumMemberDecl as node:
        node <<= Parents(T.Decl)
        node <<= Members(V.V(T.Identifier, "identifier"))
        node <<= Children(Dynamic(T.Expr, "value"))


def declareStmts(addNode):
    addNode("Stmt", loc=True, base=True)
    addNode("CompoundStmt")
    addNode("ValueStmt")
    addNode("DeclStmt")
    addNode("AttributedStmt")
    addNode("PolicyStmt")
    addNode("IfStmt")
    addNode("ForStmt")
    addNode("WhileStmt")
    addNode("LoopStmt")
    addNode("RangeStmt")
    addNode("BreakStmt")
    addNode("ContinueStmt")
    addNode("ReturnStmt")
    addNode("SyncStmt")
    addNode("AsyncStmt")
    addNode("ParallelStmt")


def defineStmts(N, T, DT):
    with N.CompoundStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Children(ManyDynamic(T.Stmt, "stmts"))
    with N.ValueStmt as node:
        node <<= Parents(T.Stmt)
    with N.DeclStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Children(Dynamic(T.Decl, "decl"))
    with N.AttributedStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Children(
            Static(T.AttrList, "attributes"),
            Dynamic(T.Stmt, "stmt"),
        )
    with N.PolicyStmt as node:
        node <<= Parents(T.AttributedStmt)
        node <<= Members(V.R(T.PolicyDecl, "policy"))
    with N.IfStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Members(V.V(T.Bool, "constexpr"))
        node <<= Children(
            Dynamic(T.Expr, "condition"),
            Dynamic(T.Stmt, "then"),
            Dynamic(T.Stmt, "else"),
        )
    with N.WhileStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Children(
            Dynamic(T.Expr, "condition"),
            Dynamic(T.CompoundStmt, "body"),
        )
    with N.ForStmt as node:
        node <<= Parents(T.AttributedStmt)
        node <<= Children(
            ManyDynamic(T.RangeStmt, "ranges"),
            Dynamic(T.CompoundStmt, "body"),
        )
    with N.LoopStmt as node:
        node <<= Parents(T.AttributedStmt)
        node <<= Children(
            ManyDynamic(T.RangeStmt, "ranges"),
            Dynamic(T.CompoundStmt, "body"),
        )
    with N.RangeStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Children(
            Dynamic(T.VarDecl, "decl"),
            Dynamic(T.DeclRefExpr, "variable"),
            Dynamic(T.RangeExpr, "range"),
        )
    with N.ReturnStmt as node:
        node <<= Parents(T.ValueStmt)
        node <<= Children(Dynamic(T.Expr, "return"))
    with N.BreakStmt as node:
        node <<= Parents(T.Stmt)
    with N.ContinueStmt as node:
        node <<= Parents(T.Stmt)
    with N.SyncStmt as node:
        node <<= Parents(T.Stmt)
    with N.AsyncStmt as node:
        node <<= Parents(T.Stmt)
    ###
    with N.ParallelStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Children(Dynamic(T.Stmt, "stmt"))


def declareExprs(addNode):
    addNode("Expr")
    addNode("LiteralExpr")
    addNode("BooleanLiteral")
    addNode("IntegerLiteral")
    addNode("FloatLiteral")
    addNode("ComplexLiteral")
    addNode("StringLiteral")
    addNode("ThisExpr")
    addNode("ParenExpr")
    addNode("UnaryOp")
    addNode("BinaryOp")
    addNode("DeclRefExpr")
    addNode("CallExpr")
    addNode("CastExpr")
    addNode("RangeExpr")
    addNode("AllocateExpr")
    addNode("ParallelExpr")
    addNode("DependExpr")
    addNode("ProvideExpr")


def defineExprs(N, T, DT):
    with N.Expr as node:
        node <<= Parents(T.ValueStmt)
        node <<= Children(Static(T.QualType, "type"))
    with N.LiteralExpr as node:
        node <<= Parents(T.Expr)
        node <<= Members(V.V(T.String, "value"))
    with N.BooleanLiteral as node:
        node <<= Parents(T.LiteralExpr)
    with N.IntegerLiteral as node:
        node <<= Parents(T.LiteralExpr)
    with N.FloatLiteral as node:
        node <<= Parents(T.LiteralExpr)
    with N.ComplexLiteral as node:
        node <<= Parents(T.LiteralExpr)
    with N.StringLiteral as node:
        node <<= Parents(T.LiteralExpr)
    with N.ThisExpr as node:
        node <<= Parents(T.Expr)
    with N.ParenExpr as node:
        node <<= Parents(T.Expr)
        node <<= Children(Dynamic(T.Expr, "expr"))
    with N.UnaryOp as node:
        node <<= Parents(T.Expr)
        node <<= Children(Dynamic(T.Expr, "expr"))
    with N.BinaryOp as node:
        node <<= Parents(T.Expr)
        node <<= Children(
            Dynamic(T.Expr, "lhs"),
            Dynamic(T.Expr, "rhs"),
        )
    with N.DeclRefExpr as node:
        node <<= Parents(T.Expr)
        node <<= Children(
            Dynamic(T.Expr, "expr"),
        )
    with N.CallExpr as node:
        node <<= Parents(T.Expr)
        node <<= Children(
            Dynamic(T.Expr, "callee"),
            ManyDynamic(T.Expr, "args"),
        )
    with N.RangeExpr as node:
        node <<= Parents(T.Expr)
        node <<= Children(
            Dynamic(T.Expr, "begin"),
            Dynamic(T.Expr, "step"),
            Dynamic(T.Expr, "end"),
        )
    with N.CastExpr as node:
        node <<= Parents(T.Expr)
        node <<= Children(Dynamic(T.Expr, "expr"))
    with N.AllocateExpr as node:
        node <<= Parents(T.Expr)
        node <<= Children(Dynamic(T.Expr, "expr"))
    with N.ParallelExpr as node:
        node <<= Parents(T.Expr)
    with N.DependExpr as node:
        node <<= Parents(T.Expr)
    with N.ProvideExpr as node:
        node <<= Parents(T.Expr)


def declareNodes(genAddNode):
    declareAttrs(genAddNode("Attr"))
    declareTypes(genAddNode("Type"))
    declareDecls(genAddNode("Decl"))
    declareStmts(genAddNode("Stmt"))
    declareExprs(genAddNode("Stmt"))


def defineNodes(N, T, DT):
    defineAttrs(N, T, DT)
    defineTypes(N, T, DT)
    defineDecls(N, T, DT)
    defineStmts(N, T, DT)
    defineExprs(N, T, DT)


def langNodes(db: LangDB):
    db.addType("DeclContext")
    db.addType("Identifier")
    db.addType("QualType")
    declareNodes(db.genAddNode)
    defineNodes(db.nodes, db.types, db.definedTypes)
    for node in db.nodes.values():
        if not node.defined:
            print(node)
