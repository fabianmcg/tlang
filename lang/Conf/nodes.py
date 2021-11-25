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


def declareNodes(addNode):
    # ******************************************************************************
    #   Attributes
    # ******************************************************************************
    addNode("Attr", loc=True)
    addNode("AttrList", loc=True)
    # ******************************************************************************
    #   Types
    # ******************************************************************************
    addNode("QualType")
    addNode("Type")
    addNode("UnresolvedType")
    addNode("DeducedType")
    addNode("AutoType")
    addNode("BuiltinType")
    addNode("SmartPtrType")
    # ******************************************************************************
    #   Declarations
    # ******************************************************************************
    addNode("Decl", loc=True)
    addNode("DeclContext", False)
    addNode("EmptyDecl")
    addNode("ImportDecl")
    addNode("ModuleDecl")
    addNode("NamedDecl")
    addNode("FunctionDecl")
    addNode("VarDecl")
    addNode("ParDecl")
    # ******************************************************************************
    #   Statements
    # ******************************************************************************
    addNode("Stmt", loc=True)
    addNode("CompoundStmt")
    addNode("NullStmt")
    addNode("ValueStmt")
    addNode("ReturnStmt")
    addNode("IfStmt")
    addNode("LoopStmt")
    addNode("DeclStmt")
    addNode("BreakStmt")
    addNode("ContinueStmt")
    # ******************************************************************************
    #   Expressions
    # ******************************************************************************
    addNode("Expr")
    addNode("DeclRefExpr")
    addNode("ThisExpr")
    addNode("ParenExpr")
    addNode("UnaryOp")
    addNode("BinaryOp")
    addNode("CallExpr")
    addNode("CastExpr")
    addNode("AllocateExpr")
    addNode("RangeExpr")
    addNode("IntegerLiteral")
    addNode("FloatLiteral")
    addNode("ComplexLiteral")
    addNode("StringLiteral")


def defineNodes(N, T):
    # ******************************************************************************
    #   Attributes
    # ******************************************************************************
    with N.AttrList as node:
        node <<= Members(V.UV(T.Attr, "attrs"))
    # ******************************************************************************
    #   Types
    # ******************************************************************************
    with N.QualType as node:
        node <<= Subtypes(
            Enumeration.create("cvr_qualifiers", [("NO_QUALS", 0), ("Const", 1), ("Reference", 2), ("Unique", 4)]),
            Enumeration.create("memory_qualifiers", [("Global", 1), ("Shared", 2), ("Local", 4), ("Constant", 8)]),
        )
        node <<= Members(
            V.UP(T.Type, "type"),
            V.V(EnumType("cvr_qualifiers", True), "qualifiers"),
            V.V(EnumType("memory_qualifiers", True), "mem_qualifiers"),
        )
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
                "numeric_precision", ["NO_NP", "I_8", "I_16", "I_32", "I_64", "F_16", "F_32", "F_64", "F_128"]
            ),
            Enumeration.create("signed_kind", ["NO_SK", "Unsigned", "Signed"]),
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
    with N.FunctionDecl as node:
        node <<= Parents(T.NamedDecl)
        node <<= Members(
            V.UV(T.ParDecl, "arguments"),
            V.UP(T.CompoundStmt, "body"),
        )
    with N.VarDecl as node:
        node <<= Parents(T.Decl)
        node <<= Members(
            V.V(T.QualType, "type"),
            V.V(T.String, "identifier"),
            V.UP(T.Expr, "init"),
        )
    with N.ParDecl as node:
        node <<= Parents(T.VarDecl)
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
    with N.LoopStmt as node:
        node <<= Parents(T.Stmt)
        node <<= Members(
            V.UV(T.RangeExpr, "range"),
            V.UP(T.CompoundStmt, "body"),
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
    with N.Expr as node:
        node <<= Parents(T.ValueStmt)
        node <<= Members(V.V(T.QualType, "type"))
    # addNode("CastExpr")
    # addNode("AllocateExpr")
    # addNode("IntegerLiteral")
    # addNode("FloatLiteral")
    # addNode("ComplexLiteral")
    # addNode("StringLiteral")


def langNodes(db: LangDB):
    T = db.types
    addNode = db.addNode
    declareNodes(addNode)
    defineNodes(db.nodes, T)
