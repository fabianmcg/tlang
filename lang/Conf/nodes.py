#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Lang.db import LangDB
from Lang.variable import variableDict as V
from Lang.type import typeDict as T


def attrNodesI(addNode, N):
    addNode("Attr")
    addNode(
        "AttrList",
        {
            "members": [V.UV(N.Attr, "attrs")],
        },
    )


def TypeNodesI(addNode, N):
    addNode(
        "Type",
        {
            "members": [V.V(N.Attr, "attrs")],
        },
    )


def declNodesI(addNode, N):
    addNode("Decl")
    addNode(
        "ModuleDecl",
        {
            "parents": N.Decl,
            "members": [
                V.VV(N.Decl, "decls"),
            ],
        },
    )
    addNode(
        "NamedDecl",
        {
            "parents": N.Decl,
            "members": [
                V.V(T.S, "identifier"),
            ],
        },
    )


def stmtNodesI(addNode, N):
    addNode("Stmt")
    addNode(
        "CompoundStmt",
        {
            "parents": N.Stmt,
            "members": [
                V.VV(N.Stmt, "stmts"),
            ],
        },
    )


def declNodesII(addNode, N):
    addNode(
        "Function",
        {
            "parents": N.NamedDecl,
            "members": [
                V.A("arguments"),
                V.V(N.CompoundStmt, "body"),
            ],
        },
    )


def langNodes(db: LangDB):
    N = db.getNodesIdentifiers()
    addNode = db.addNode
    attrNodesI(addNode, N)
    declNodesI(addNode, N)
    stmtNodesI(addNode, N)
    declNodesII(addNode, N)
