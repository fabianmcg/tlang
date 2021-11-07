#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Lang.node import Node
from Lang.rule import Match
from Lang.db import addNode, astNodes, RTok
from Lang.variable import variableDict as V

ast = astNodes

addNode(Node, "Attr")
addNode(
    Node,
    "AttrList",
    {
        "members": [V.UV("Attr", "attrs")],
    },
)

addNode(Node, "Decl")
addNode(Node, "DeclGroup")
addNode(Node, "ModuleDecl")


addNode(Node, "Stmt")
addNode(
    Node,
    "CompoundStmt",
    {
        "parents": "Stmt",
        "members": [
            V.V("Stmt", "stmts"),
        ],
    },
)

addNode(
    Node,
    "Function",
    {
        "parents": "Decl",
        "members": [
            V.A("arguments"),
            V.V("CompoundStmt", "body"),
        ],
    },
)
