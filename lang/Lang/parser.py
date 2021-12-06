#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from pyparsing import Forward, MatchFirst, Optional, Keyword, Literal, delimitedList
from Utility.parsing import *
from Utility.parsing import punctuation as P
from Lang.type import TypeHelper
from Lang.struct import *


class Parser:
    Int, Size, String, Vector, Unique, Reference = suppressKeywords(
        "int", "size", "string", "vector", "unique", "reference"
    )

    @staticmethod
    def createParents():
        parents = suppressKeywords("parents")
        parentList = Optional(delimitedList(P.identifier))
        return parseGroup(parents + P.colon + P.lbracket + parentList + P.rbracket + P.semi)

    @staticmethod
    def createRecords():
        lbracket, rbracket = suppressLiterals(":{", "}:")
        return parseGroup(lbracket + ... + rbracket)

    @staticmethod
    def createBaseType():
        types = list(keywordList(*TypeHelper.getBaseTypenames())) + [P.identifier]

        def action(x):
            return TypeHelper.getType(str(x[0]))()

        return parseElement(MatchFirst(types), action)

    @staticmethod
    def createTemplateType():
        langle, rangle = suppressChars("<>")
        types = list(keywordList(*TypeHelper.getTemplateTypenames())) + [P.identifier]

        def action(x):
            return TypeHelper.getType(str(x[0]), True)(*x[1:])

        templateType = parseElement(Forward(), action)
        baseType = templateType | Parser.createBaseType()
        templateType <<= MatchFirst(types) + langle + delimitedList(baseType) + rangle
        return templateType

    @staticmethod
    def createType():
        return Parser.createTemplateType() | Parser.createBaseType()

    @staticmethod
    def createMember():
        Type = Parser.createType()
        Variable = Type + P.identifier
        return parseGroup(delimitedList(Variable))

    @staticmethod
    def createMembers():
        lbracket, rbracket, colon, semi = suppressChars("[]:;")
        members = suppressLiterals("members")
        membertList = Optional(delimitedList(Parser.createMember()))
        return parseGroup(members + colon + lbracket + membertList + rbracket + semi)

    @staticmethod
    def createChildren():
        lbracket, rbracket, colon, semi = suppressChars("[]:;")
        members = suppressLiterals("children")
        membertList = Optional(delimitedList(Parser.createMember()))
        return parseGroup(members + colon + lbracket + membertList + rbracket + semi)

    @staticmethod
    def createHeader():
        langle, rangle = suppressChars("<>")
        return parseGroup(langle + P.identifier + rangle)

    @staticmethod
    def createNode():
        node = suppressKeywords("node")
        header = Parser.createHeader()
        parents = Parser.createParents()
        members = Parser.createMembers()
        body = parseGroup(
            P.lbrace + pp.Optional(parents, default=[]) + pp.Optional(members, default=[]) + P.rbrace + P.semi
        )

        def nodeAction(x):
            return Node.createFromParse(x[0])

        return parseGroup(node + P.identifier + pp.Optional(header, default=[]) + (body | P.semi), nodeAction)

    @staticmethod
    def createTop():
        top = Parser.createNode()
        return pp.OneOrMore(top).ignore(pp.cStyleComment)

    @staticmethod
    def parse(filename):
        pp._enable_all_warnings()
        result = Parser.createTop().parseFile(filename, parseAll=True)
        # print(result.dump())
        print("\n".join(map(str, list(result))))


def parse(filename):
    parser = Parser.parse(filename)
