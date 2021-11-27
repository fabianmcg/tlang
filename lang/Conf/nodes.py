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


def declareNodes(genAddNode):
    # ******************************************************************************
    #   Attributes
    # ******************************************************************************
    addNode = genAddNode("Attr")
    addNode("Attr", loc=True)
    addNode("AttrList", loc=True)
    # ******************************************************************************
    #   Types
    # ******************************************************************************
    addNode = genAddNode("Type")
    addNode("Type")
    addNode("UnresolvedType")
    addNode("DeducedType")
    addNode("AutoType")
    addNode("BuiltinType")
    addNode("SmartPtrType")
    # addNode("QualType")
    # ******************************************************************************
    #   Declarations
    # ******************************************************************************
    addNode = genAddNode("Decl")
    addNode("Decl", loc=True)
    # addNode("DeclContext", False)
    addNode("EmptyDecl")
    addNode("ModuleDecl")
    addNode("ImportDecl")
    addNode("NamedDecl")
    addNode("EnvironmentDecl")
    addNode("PolicyDecl")
    addNode("StructDecl")
    addNode("EnumDecl")
    addNode("FunctorDecl")
    addNode("FunctionDecl")
    addNode("MethodDecl")
    addNode("VarDecl")
    addNode("ParDecl")
    addNode("MemberDecl")
    # ******************************************************************************
    #   Statements
    # ******************************************************************************
    addNode = genAddNode("Stmt")
    addNode("Stmt", loc=True)
    addNode("CompoundStmt")
    addNode("ValueStmt")
    addNode("NullStmt")
    addNode("DeclStmt")
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
    addNode("DependStmt")
    addNode("ProvideStmt")
    # ******************************************************************************
    #   Expressions
    # ******************************************************************************
    addNode("Expr")
    addNode("LiteralExpr")
    addNode("BooleanLiteral")
    addNode("IntegerLiteral")
    addNode("FloatLiteral")
    addNode("ComplexLiteral")
    addNode("StringLiteral")
    addNode("DeclRefExpr")
    addNode("ThisExpr")
    addNode("ParenExpr")
    addNode("UnaryOp")
    addNode("BinaryOp")
    addNode("CallExpr")
    addNode("CastExpr")
    addNode("RangeExpr")
    addNode("AllocateExpr")
    addNode("ParallelExpr")
    addNode("DependExpr")
    addNode("ProvideExpr")


def defineNodes(N, T):
    # ******************************************************************************
    #   Attributes
    # ******************************************************************************
    with N.AttrList as node:
        node <<= Members(V.UV(T.Attr, "attributes"))
    # ******************************************************************************
    #   Types
    # ******************************************************************************
    # with N.QualType as node:
    #     node <<= Subtypes(
    #         Enumeration.create("cvr_qualifiers", [("No_quals", 0), ("Const", 1), ("Reference", 2)]),
    #         Enumeration.create("memory_qualifiers", [("Local", 1), ("Global", 2), ("Shared", 3), ("Constant", 8)]),
    #     )
    #     node <<= Members(
    #         V.UP(T.Type, "type"),
    #         V.V(EnumType("cvr_qualifiers", True), "qualifiers"),
    #         V.V(EnumType("memory_qualifiers", True), "mem_qualifiers"),
    #     )
    with N.UnresolvedType as node:
        node <<= Parents(T.Type)
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
    with N.SmartPtrType as node:
        node <<= Parents(T.Type)
        node <<= Members(V.UP(T.Type, "underlying"))
    # ******************************************************************************
    #   Declarations
    # ******************************************************************************
    with N.EmptyDecl as node:
        node <<= Parents(T.Decl)
    with N.ImportDecl as node:
        node <<= Parents(T.Decl)
        node <<= Members(V.V(T.String, "module"))
    with N.ModuleDecl as node:
        node <<= Parents(T.Decl, T.DeclContext)
        node <<= Members(V.UV(T.Decl, "decls"))
    with N.NamedDecl as node:
        node <<= Parents(T.Decl)
        node <<= Members(V.V(T.String, "identifier"))
    with N.VarDecl as node:
        node <<= Parents(T.Decl)
        node <<= Members(
            V.V(T.QualType, "type"),
            V.V(T.String, "identifier"),
            V.UP(T.Expr, "init"),
        )
    with N.ParDecl as node:
        node <<= Parents(T.VarDecl)
    with N.MemberDecl as node:
        node <<= Parents(T.VarDecl)
    with N.EnvironmentDecl as node:
        node <<= Parents(T.NamedDecl)
    with N.PolicyDecl as node:
        node <<= Parents(T.NamedDecl)
    with N.StructDecl as node:
        node <<= Parents(T.NamedDecl)
        node <<= Members(
            V.V(T.AttrList, "attributes"),
            V.UV(T.MethodDecl, "methods"),
            V.UV(T.MemberDecl, "members"),
        )
    with N.EnumDecl as node:
        node <<= Parents(T.NamedDecl)
        node <<= Members(V.V(T.QualType, "type"))
    with N.FunctorDecl as node:
        node <<= Parents(T.NamedDecl)
        node <<= Members(
            V.UV(T.AttrList, "attributes"),
            V.UV(T.ParDecl, "arguments"),
            V.UP(T.CompoundStmt, "body"),
        )
    with N.FunctionDecl as node:
        node <<= Parents(T.FunctorDecl)
    with N.MethodDecl as node:
        node <<= Parents(T.FunctorDecl)
    # ******************************************************************************
    #   Statements
    # ******************************************************************************
    with N.CompoundStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Members(V.UV(T.Stmt, "stmts"))
    with N.NullStmt as node:
        node <<= Parents(T.Stmt)
    with N.ValueStmt as node:
        node <<= Parents(T.Stmt)
    with N.ReturnStmt as node:
        node <<= Parents(T.ValueStmt)
        node <<= Members(
            V.UP(T.Expr, "return"),
        )
    with N.WhileStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Members(
            V.UV(T.Expr, "condition"),
            V.UP(T.CompoundStmt, "body"),
        )
    with N.ForStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Members(
            V.UV(T.RangeStmt, "ranges"),
            V.UP(T.CompoundStmt, "body"),
        )
    with N.LoopStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Members(
            V.UV(T.RangeStmt, "ranges"),
            V.UP(T.CompoundStmt, "body"),
        )
    with N.RangeStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Members(
            V.UV(T.RangeStmt, "ranges"),
            V.UP(T.RangeExpr, "range"),
        )
    with N.IfStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Members(
            V.UP(T.Expr, "condition"),
            V.UP(T.Stmt, "then"),
            V.UP(T.Stmt, "else"),
        )
    with N.DeclStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Members(
            V.UP(T.Decl, "decl"),
        )
    with N.BreakStmt as node:
        node <<= Parents(T.Stmt)
    with N.ContinueStmt as node:
        node <<= Parents(T.Stmt)
    with N.SyncStmt as node:
        node <<= Parents(T.Stmt)
    with N.AsyncStmt as node:
        node <<= Parents(T.Stmt)
    with N.ParallelStmt as node:
        node <<= Parents(T.Stmt)
    with N.DependStmt as node:
        node <<= Parents(T.Stmt)
    with N.ProvideStmt as node:
        node <<= Parents(T.Stmt)
    # ******************************************************************************
    #   Expressions
    # ******************************************************************************
    with N.Expr as node:
        node <<= Parents(T.ValueStmt)
        node <<= Members(V.V(T.QualType, "type"))
    with N.DeclRefExpr as node:
        node <<= Parents(T.Expr)
        node <<= Members(
            V.UP(T.Expr, "expr"),
        )
    with N.ThisExpr as node:
        node <<= Parents(T.Expr)
    with N.ParenExpr as node:
        node <<= Parents(T.Expr)
        node <<= Members(
            V.UP(T.Expr, "expr"),
        )
    with N.UnaryOp as node:
        node <<= Parents(T.Expr)
        node <<= Members(
            V.UP(T.Expr, "expr"),
        )
    with N.BinaryOp as node:
        node <<= Parents(T.Expr)
        node <<= Members(
            V.UP(T.Expr, "lhs"),
            V.UP(T.Expr, "rhs"),
        )
    with N.CallExpr as node:
        node <<= Parents(T.Expr)
        node <<= Members(
            V.UP(T.Expr, "callee"),
            V.UV(T.Expr, "args"),
        )
    with N.RangeExpr as node:
        node <<= Parents(T.Expr)
        node <<= Members(
            V.UP(T.Expr, "begin"),
            V.UP(T.Expr, "step"),
            V.UP(T.Expr, "end"),
        )
    with N.LiteralExpr as node:
        node <<= Parents(T.Expr)
        node <<= Members(V.V(T.QualType, "type"))
        node <<= Members(V.V(T.String, "value"))
    with N.IntegerLiteral as node:
        node <<= Parents(T.LiteralExpr)
    with N.BooleanLiteral as node:
        node <<= Parents(T.LiteralExpr)
    with N.FloatLiteral as node:
        node <<= Parents(T.LiteralExpr)
    with N.StringLiteral as node:
        node <<= Parents(T.LiteralExpr)
    with N.CastExpr as node:
        node <<= Parents(T.Expr)
        node <<= Members(V.V(T.QualType, "type"))
        node <<= Members(V.UP(T.Expr, "expr"))
    with N.AllocateExpr as node:
        node <<= Parents(T.Expr)
        node <<= Members(V.V(T.QualType, "type"))
        node <<= Members(V.UP(T.Expr, "expr"))
    with N.ParallelExpr as node:
        node <<= Parents(T.Expr)
    with N.DependExpr as node:
        node <<= Parents(T.Expr)
    with N.ProvideExpr as node:
        node <<= Parents(T.Expr)


def langNodes(db: LangDB):
    T = db.types
    declareNodes(db.genAddNode)
    defineNodes(db.nodes, T)
