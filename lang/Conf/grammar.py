#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Lang.type import typeDict as Type
from Lang.variable import variableDict as V
from Lang.rule import ruleDict as R
from Lang.db import tokens as T, nodes as N, instructionDict as I, helperRules as H
from Conf.nodes import astNodes as A

H["ArgumentList"] = (
    I.VD(V.UV(A.Decl, "args")) + (I.VR("args") ** N.Decl) + R.ZM(T.Comma + (I.VR("args") ** N.Decl)) + I.RET("args")
)

A.Function <<= (
    T.Function
    + (I.VR("identifier") << T.Identifier)
    + T.LParen
    + ~(H.ArgumentList)
    + T.RParen
    + (I.VR("body") ** N.CompoundStmt)
)
