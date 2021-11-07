#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.dotDict import *
from Lang.rule import Match

from Conf.toks import LToks

astNodes = DotDict()


RTok = ApplyDotDict(lambda x: Match(x), LToks)


def addNode(kind, identifier, dct={}):
    astNodes[identifier] = kind(identifier, **dct)
