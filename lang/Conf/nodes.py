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


def declNodesII(addNode, T):
    with addNode("Function") as node:
        node <<= Parents(T.NamedDecl)
        node <<= Members(
            V.A("arguments"),
            V.V(T.CompoundStmt, "body"),
        )


def langNodes(db: LangDB):
    T = db.types
    addNode = db.addNode
    attrNodesI(addNode, T)
    declNodesI(addNode, T)
    stmtNodesI(addNode, T)
    declNodesII(addNode, T)
