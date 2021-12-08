#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

import pyparsing as pp

from Utility.dotDict import ApplyDotDict


def suppressList(Type, *literals):
    if len(literals) > 1:
        return map(lambda x: Type(x).suppress(), literals)
    return Type(literals[0]).suppress()


def parsingList(Type, *literals):
    if len(literals) > 1:
        return map(lambda x: Type(x), literals)
    return Type(literals[0])


def suppressChars(chars):
    if len(chars) > 1:
        return map(lambda x: pp.Char(x).suppress(), chars)
    return pp.Char(chars).suppress()


def suppressLiterals(*literals):
    return suppressList(pp.Literal, *literals)


def suppressKeywords(*kws):
    return suppressList(pp.Keyword, *kws)


def keywordList(*kws):
    return parsingList(pp.Keyword, *kws)


def parseElement(parseElement, action=None):
    if action == None:
        return parseElement
    x = parseElement.copy()
    x.setParseAction(action)
    return x


def parseGroup(parseElement, action=None):
    if action == None:
        return pp.Group(parseElement)
    x = pp.Group(parseElement.copy())
    x.setParseAction(action)
    return x


def parseOptional(parseElement, action=None, default=None):
    if default == None:
        default = []
    if action == None:
        return pp.Optional(parseElement, default=default)
    x = pp.Optional(parseElement.copy(), default=default)
    x.setParseAction(action)
    return x


punctuation = ApplyDotDict(
    lambda x: x.copy(),
    {
        "identifier": pp.Word(pp.alphas + "_", pp.alphanums + "_"),
        "number": pp.Word(pp.nums) | pp.Word("0x", pp.hexnums),
        "lbrace": suppressChars("{"),
        "rbrace": suppressChars("}"),
        "lbracket": suppressChars("["),
        "rbracket": suppressChars("]"),
        "lparen": suppressChars("("),
        "rparen": suppressChars(")"),
        "langle": suppressChars("<"),
        "rangle": suppressChars(">"),
        "colon": suppressChars(":"),
        "semi": suppressChars(";"),
        "at": suppressChars("@"),
        "equal": suppressLiterals(":="),
        "c_lbrace": suppressLiterals(":{"),
        "c_rbrace": suppressLiterals("}:"),
        "c_lbracket": suppressLiterals(":["),
        "c_rbracket": suppressLiterals("]:"),
        "a_langle": suppressLiterals("@<"),
        "a_rangle": suppressLiterals(">@"),
    },
)
