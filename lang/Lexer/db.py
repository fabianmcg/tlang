#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.dotDict import DotDict
from Lexer.token import Definition

lexerDefinitions = DotDict()
lexerTokens = DotDict()


def addToken(kind, identifier, *rules):
    lexerTokens[identifier] = kind(identifier, *rules)


def addDefinition(identifier, *rules):
    lexerDefinitions[identifier] = Definition(identifier, *rules)
