#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.type import typeDict as Type
from Utility.variable import variableDict as V
from Lang.db import LangDB
from Grammar.rule import ZeroOrMore as ZM, VariableRef as VarRef


def grammar(db: LangDB):
    R, N, T = db.getGrammar()

    # A.Type <<= T.Int | T.Float | T.Void | T.Identifier

    # H["ArgumentList"] = (
    #     I.VD(V.UV(A.Decl, "args")) + (I.VR("args") ** N.Decl) + R.ZM(T.Comma + (I.VR("args") ** N.Decl)) + I.RET("args")
    # )

    R.CompoundStmt <<= T.LBrace + ZM(VarRef("stmts") ** N.Stmt) + T.RBrace

    R.Function <<= (
        T.Function + (VarRef("identifier") << T.Identifier) + T.LParen + T.RParen + (VarRef("body") << N.CompoundStmt)
    )

    R.IfStmt <<= T.If + T.LParen + N.Expr + T.RParen + (N.Stmt | (N.Stmt + T.Else + N.Stmt))
