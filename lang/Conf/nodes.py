#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Lang.node import Node
from Lang.db import addNode, astNodes as A, nodesTypes as NT
from Lang.variable import variableDict as V

astNodes = A

addNode(Node, "Attr")
addNode(
    Node,
    "AttrList",
    {
        "members": [V.UV(NT.Attr, "attrs")],
    },
)

addNode(Node, "Decl")
addNode(
    Node,
    "DeclGroup",
    {
        "members": [V.UV(NT.Decl, "decls")],
    },
)
addNode(
    Node,
    "ModuleDecl",
    {
        "parents": "Decl",
        "members": [
            V.V("Stmt", "stmts"),
        ],
    },
)


addNode(Node, "Stmt")
addNode(
    Node,
    "CompoundStmt",
    {
        "parents": "Stmt",
        "members": [
            V.V(NT.Stmt, "stmts"),
        ],
    },
)

addNode(
    Node,
    "Function",
    {
        "parents": "Decl",
        "members": [
            V.A("identifier"),
            V.A("arguments"),
            V.V(NT.CompoundStmt, "body"),
        ],
    },
)
