#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.type import typeDict as Type
from Utility.variable import variableDict as V
from Lang.db import LangDB
from Grammar.rule import ZeroOrMore as ZM, EmptyProduction as Empty
from Grammar.instruction import VariableRef as VarRef


def grammar(db: LangDB):
    R, N, T = db.getGrammar()
    # ******************************************************************************
    #
    #   Type
    #
    # ******************************************************************************

    # ******************************************************************************
    #
    #   Declarations
    #
    # ******************************************************************************
    R.FunctionDecl <<= (
        T.Function + (VarRef("identifier") << T.Identifier) + T.LParen + T.RParen + (VarRef("body") << N.CompoundStmt)
    )
    # ******************************************************************************
    #
    #   Statements
    #
    # ******************************************************************************
    R.CompoundStmt <<= T.LBrace + ZM(VarRef("stmts") ** N.Stmt) + T.RBrace
    R.IfStmt <<= T.If + T.LParen + N.Expr + T.RParen + N.Stmt + ((T.Else + N.Stmt) | Empty())

    # ******************************************************************************
    #
    #   Expressions
    #
    # ******************************************************************************
    R.Expr <<= N.BinaryOp | N.ParenExpr | N.ThisExpr | N.DeclRefExpr
    R.BinaryOp <<= N.Expr + (T.Plus | T.Minus | T.Multiply | T.Divide) + N.Expr
    R.ParenExpr <<= T.LParen + N.Expr + T.RParen
    R.DeclRefExpr <<= T.Identifier
