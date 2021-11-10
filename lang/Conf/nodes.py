#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Lang.db import LangDB
from Lang.variable import variableDict as V


def langNodes(db: LangDB):
    NT = db.getNodesTypes()
    db.addNode("Attr")
    db.addNode(
        "AttrList",
        {
            "members": [V.UV(NT.Attr, "attrs")],
        },
    )
    db.addNode("Decl")
    db.addNode(
        "DeclGroup",
        {
            "members": [V.UV(NT.Decl, "decls")],
        },
    )
    db.addNode(
        "ModuleDecl",
        {
            "parents": "Decl",
            "members": [
                V.V("Stmt", "stmts"),
            ],
        },
    )
    db.addNode("Stmt")
    db.addNode(
        "CompoundStmt",
        {
            "parents": "Stmt",
            "members": [
                V.V(NT.Stmt, "stmts"),
            ],
        },
    )
    db.addNode(
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
