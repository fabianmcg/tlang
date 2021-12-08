#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from pyparsing import Forward, MatchFirst, Optional, delimitedList
from Utility.parsing import *
from Utility.parsing import punctuation as P
from Cxx.type import TypeHelper
from Cxx.variable import ChildNode, DynamicList, DynamicNode, StaticList, StaticNode, Variable
from Cxx.struct import *
from Lang.nodes import *


class Parser:
    @staticmethod
    def createSection():
        ld, rd = suppressLiterals(*CodeSection.delimeters)
        return parseElement(ld + ... + rd, lambda x: CodeSection(x[0]))

    @staticmethod
    def createHeaderSection():
        ld, rd = suppressLiterals(*HeaderSection.delimeters)
        return parseElement(ld + ... + rd, lambda x: HeaderSection(x[0]))

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

        def action(x):
            return Variable(*x)

        variable = parseElement(Type + P.identifier, action)
        return variable

    @staticmethod
    def createChild():
        static, list = keywordList("static", "list")

        def actionType(x):
            s = len(x[0]) > 0
            l = len(x[1]) > 0
            if s and l:
                return StaticList(x[2])
            elif not s and l:
                return DynamicList(x[2])
            elif s and not l:
                return StaticNode(x[2])
            else:
                return DynamicNode(x[2])

        Type = parseElement(parseOptional(static) + parseOptional(list) + P.identifier, actionType)

        def action(x):
            print(x)
            return ChildNode(*x)

        variable = parseElement(Type + P.identifier, action)
        return variable

    @staticmethod
    def createParents():
        parents = suppressKeywords("parents")
        parentList = Optional(delimitedList(P.identifier), default=[])
        return parseGroup(parents + P.colon + P.lbracket + parentList + P.rbracket + P.semi)

    @staticmethod
    def createMembers():
        members = suppressLiterals("members")
        membertList = Optional(delimitedList(Parser.createMember()), default=[])
        return parseGroup(members + P.colon + P.lbracket + membertList + P.rbracket + P.semi)

    @staticmethod
    def createChildren():
        lbracket, rbracket, colon, semi = suppressChars("[]:;")
        members = suppressLiterals("children")
        membertList = Optional(delimitedList(Parser.createChild()))
        return parseGroup(members + colon + lbracket + membertList + rbracket + semi, print)

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
        children = Parser.createChildren()
        headerSection = Parser.createHeaderSection()
        epilogueSection = Parser.createSection()
        body = parseGroup(
            P.lbrace
            + parseOptional(parents)
            + parseOptional(members)
            + parseOptional(children)
            + parseOptional(headerSection)
            + parseOptional(epilogueSection)
            + P.rbrace
            + P.semi
        )

        def nodeAction(x):
            return Node.createFromParse(x[0])

        return parseGroup(node + P.identifier + parseOptional(header) + (body | P.semi), nodeAction)

    @staticmethod
    def createTopDecl():
        top = Parser.createNode()
        return pp.OneOrMore(top)

    @staticmethod
    def createTop(db: ASTDatabase):
        file = suppressKeywords("file")
        ld, rd = suppressLiterals(*FileNamespace.delimeters)

        def action(x):
            return db.addFileNamespace(FileNamespace(str(x[0]), x[1:]))

        fileNamespace = parseElement(file + P.identifier + ld + Parser.createTopDecl() + rd, action)
        return pp.OneOrMore(fileNamespace).ignore(pp.cStyleComment)

    @staticmethod
    def parse(filename):
        pp._enable_all_warnings()
        db = ASTDatabase()
        Parser.createTop(db).ignore(pp.cStyleComment).parseFile(filename, parseAll=True)
        print(db)


def parse(filename):
    parser = Parser.parse(filename)
