#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Lang.db import LangDB
from Utility.variable import variableDict as V
from Utility.struct import ClassMembers as Members, ClassParents as Parents


def declareNodes(addNode):
    # ******************************************************************************
    #   Attributes
    # ******************************************************************************
    addNode("Attr")
    addNode("AttrList")
    # ******************************************************************************
    #   Types
    # ******************************************************************************
    addNode("Type")
    # ******************************************************************************
    #   Declarations
    # ******************************************************************************
    addNode("Decl")
    addNode("DeclContext")
    addNode("ModuleDecl")
    addNode("NamedDecl")
    addNode("FunctionDecl")
    # ******************************************************************************
    #   Statements
    # ******************************************************************************
    addNode("Stmt")
    addNode("CompoundStmt")
    addNode("NullStmt")
    addNode("ValueStmt")
    addNode("ReturnStmt")
    addNode("LoopStmt")
    addNode("IfStmt")
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
    addNode("RangeExpr")


def defineNodes(N, T):
    # ******************************************************************************
    #   Attributes
    # ******************************************************************************
    with N.AttrList as node:
        node <<= Members(V.UV(T.Attr, "attrs"))
    # ******************************************************************************
    #   Declarations
    # ******************************************************************************
    with N.ModuleDecl as node:
        node <<= Parents(T.DeclContext)
        node <<= Members(V.UV(T.Decl, "decls"))
    with N.NamedDecl as node:
        node <<= Parents(T.Decl)
        node <<= Members(V.V(T.String, "identifier"))
    with N.FunctionDecl as node:
        node <<= Parents(T.NamedDecl)
        node <<= Members(
            V.A("arguments"),
            V.UP(T.CompoundStmt, "body"),
        )
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
    # ******************************************************************************
    #   Expressions
    # ******************************************************************************
    with N.Expr as node:
        node <<= Parents(T.ValueStmt)
    with N.DeclRefExpr as node:
        node <<= Parents(T.Expr)
        node <<= Members(
            V.UP(T.Expr, "expr"),
        )
    with N.ThisExpr as node:
        node <<= Parents(T.Expr)
        node <<= Members(
            V.UP(T.Expr, "expr"),
        )
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


def langNodes(db: LangDB):
    T = db.types
    addNode = db.addNode
    declareNodes(addNode)
    defineNodes(db.nodes, T)
