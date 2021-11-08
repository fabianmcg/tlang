#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Lang.db import toks as T, nodes as N, vars as V
from Conf.nodes import astNodes as A
from Lang.rule import *


A.Function <<= T.Function + (V.Function.identifier << T.Identifier) + T.LParen + T.RParen + N.CompoundStmt
