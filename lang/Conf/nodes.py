#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Lang.db import LangDB
from Utility.variable import variableDict as V
from Utility.struct import ClassMembers as Members, ClassParents as Parents


def attrNodesI(addNode, T):
    addNode("Attr")
    with addNode("AttrList") as node:
        node <<= Members(V.UV(T.Attr, "attrs"))


def typeNodes(addNode, T):
    addNode("Type")


def declNodesI(addNode, T):
    addNode("Decl")
    with addNode("ModuleDecl") as node:
        node <<= Parents(T.Decl)
        node <<= Members(V.VV(T.Decl, "decls"))
    with addNode("NamedDecl") as node:
        node <<= Parents(T.Decl)
        node <<= Members(V.V(T.String, "identifier"))


def stmtNodesI(addNode, T):
    addNode("Stmt")
    with addNode("CompoundStmt") as node:
        node <<= Parents(T.Stmt)
        node <<= Members(V.VV(T.Stmt, "stmts"))
    with addNode("NullStmt") as node:
        node <<= Parents(T.Stmt)
    with addNode("ValueStmt") as node:
        node <<= Parents(T.Stmt)


def declNodesII(addNode, T):
    with addNode("Function") as node:
        node <<= Parents(T.NamedDecl)
        node <<= Members(
            V.A("arguments"),
            V.UP(T.CompoundStmt, "body"),
        )


def exprNodesI(addNode, T):
    with addNode("Expr") as node:
        node <<= Parents(T.ValueStmt)
    with addNode("DeclRefExpr") as node:
        node <<= Parents(T.Expr)
        node <<= Members(
            V.UP(T.Expr, "expr"),
        )
    with addNode("ThisExpr") as node:
        node <<= Parents(T.Expr)
        node <<= Members(
            V.UP(T.Expr, "expr"),
        )
    with addNode("UnaryOp") as node:
        node <<= Parents(T.Expr)
        node <<= Members(
            V.UP(T.Expr, "expr"),
        )
    with addNode("BinaryOp") as node:
        node <<= Parents(T.Expr)
        node <<= Members(
            V.UP(T.Expr, "lhs"),
            V.UP(T.Expr, "rhs"),
        )
    with addNode("CallExpr") as node:
        node <<= Parents(T.Expr)
        node <<= Members(
            V.UP(T.Expr, "callee"),
            V.UV(T.Expr, "args"),
        )
    with addNode("RangeExpr") as node:
        node <<= Parents(T.Expr)
        node <<= Members(
            V.UP(T.Expr, "begin"),
            V.UP(T.Expr, "step"),
            V.UP(T.Expr, "end"),
        )


def stmtNodesII(addNode, T):
    with addNode("LoopStmt") as node:
        node <<= Parents(T.Stmt)
        node <<= Members(
            V.UV(T.RangeExpr, "range"),
            V.UP(T.CompoundStmt, "body"),
        )
    with addNode("IfStmt") as node:
        node <<= Parents(T.Stmt)
        node <<= Members(
            V.UP(T.Expr, "condition"),
            V.UP(T.Stmt, "then"),
            V.UP(T.Stmt, "else"),
        )
    with addNode("ReturnStmt") as node:
        node <<= Parents(T.Stmt)
        node <<= Members(
            V.UP(T.Expr, "return"),
        )


def langNodes(db: LangDB):
    T = db.types
    addNode = db.addNode
    attrNodesI(addNode, T)
    typeNodes(addNode, T)
    declNodesI(addNode, T)
    stmtNodesI(addNode, T)
    declNodesII(addNode, T)
    exprNodesI(addNode, T)
    stmtNodesII(addNode, T)
