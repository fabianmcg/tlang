#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from pyparsing import Forward, MatchFirst, OneOrMore, Optional, ZeroOrMore, delimitedList
from Utility.parsing import *
from Utility.parsing import punctuation as P
from Cxx.type import TypeHelper
from Cxx.variable import Variable
from Cxx.struct import *
from AST.ast import *
from AST.astNode import *
from Utility.util import objetOrNone


class parseTypes:
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
        baseType = templateType | parseTypes.createBaseType()
        templateType <<= MatchFirst(types) + langle + delimitedList(baseType) + rangle
        return templateType

    @staticmethod
    def createType():
        return parseTypes.createTemplateType() | parseTypes.createBaseType()


class parseEnum:
    @staticmethod
    def createEnumMember():
        equal = suppressChars("=")

        def action(x):
            return EnumElement(x[0], x[1] if len(x[1]) else None)

        enum = parseElement(P.identifier + parseOptional(equal + P.number), action)
        return enum

    @staticmethod
    def createEnum(insideClass=False):
        enum = suppressKeywords("enum")
        kw_class = keywordList("class")

        def action(x):
            return Enum(x[1], len(x[0]) > 0, x[2], insideClass=insideClass)

        membertList = parseGroup(delimitedList(parseEnum.createEnumMember()))
        return parseElement(
            enum + parseOptional(kw_class) + P.identifier + P.lbrace + membertList + P.rbrace + P.semi, action
        )

    @staticmethod
    def createEnums(insideClass=False):
        return parseGroup(OneOrMore(parseEnum.createEnum(insideClass)))


class parseNode:
    @staticmethod
    def createSection():
        ld, rd = suppressLiterals(*CodeSection.delimeters)
        return parseElement(ld + ... + rd, lambda x: CodeSection(x[0]))

    @staticmethod
    def createHeaderSection():
        ld, rd = suppressLiterals(*HeaderSection.delimeters)
        return parseElement(ld + ... + rd, lambda x: HeaderSection(x[0]))

    @staticmethod
    def createMember():
        Type = parseTypes.createType()

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
        membertList = Optional(delimitedList(parseNode.createMember()), default=[])
        return parseGroup(members + P.colon + P.lbracket + membertList + P.rbracket + P.semi)

    @staticmethod
    def createChildren():
        lbracket, rbracket, colon, semi = suppressChars("[]:;")
        members = suppressLiterals("children")
        membertList = Optional(delimitedList(parseNode.createChild()))
        return parseGroup(members + colon + lbracket + membertList + rbracket + semi)

    @staticmethod
    def createHeader():
        langle, rangle = suppressChars("<>")
        return parseGroup(langle + P.identifier + rangle)


class Parser:
    @staticmethod
    def createNode():
        node = suppressKeywords("node")
        header = parseNode.createHeader()
        parents = parseNode.createParents()
        members = parseNode.createMembers()
        enums = parseEnum.createEnums(True)
        children = parseNode.createChildren()
        headerSection = parseNode.createHeaderSection()
        epilogueSection = parseNode.createSection()
        body = parseGroup(
            P.lbrace
            + parseOptional(parents)
            + parseOptional(members)
            + parseOptional(children)
            + parseOptional(enums)
            + parseOptional(headerSection)
            + parseOptional(epilogueSection)
            + P.rbrace
            + P.semi
        )

        def nodeAction(x):
            x = x[0]
            y = x[2]
            classOf = x[1][0] if objetOrNone(x[1]) else None
            parents = objetOrNone(y[0])
            members = objetOrNone(y[1])
            children = objetOrNone(y[2])
            enums = objetOrNone(y[3])
            headerSection = objetOrNone(y[4])
            epilogueSection = objetOrNone(y[5])
            return Node.createFromParse(
                x[0],
                classOf=classOf,
                parents=parents,
                members=members,
                children=children,
                enums=enums,
                headerSection=headerSection,
                epilogueSection=epilogueSection,
            )

        return parseGroup(node + P.identifier + parseOptional(header) + body, nodeAction)

    @staticmethod
    def createStruct():
        node = suppressKeywords("struct")
        parents = parseNode.createParents()
        members = parseNode.createMembers()
        enums = parseEnum.createEnums(True)
        headerSection = parseNode.createHeaderSection()
        epilogueSection = parseNode.createSection()
        body = parseGroup(
            P.lbrace
            + parseOptional(parents)
            + parseOptional(members)
            + parseOptional(enums)
            + parseOptional(headerSection)
            + parseOptional(epilogueSection)
            + P.rbrace
            + P.semi
        )

        def nodeAction(x):
            x = x[0]
            y = x[1]
            parents = objetOrNone(y[0])
            members = objetOrNone(y[1])
            enums = objetOrNone(y[2])
            headerSection = objetOrNone(y[3])
            epilogueSection = objetOrNone(y[4])
            return Struct.createFromParse(
                x[0],
                parents=parents,
                members=members,
                enums=enums,
                headerSection=headerSection,
                epilogueSection=epilogueSection,
            )

        return parseGroup(node + P.identifier + body, nodeAction)

    @staticmethod
    def createTopDecl():
        top = Parser.createNode() | Parser.createStruct() | parseEnum.createEnum()
        return Optional(parseNode.createHeaderSection()) + pp.OneOrMore(top) + Optional(parseNode.createSection())

    @staticmethod
    def createTop(db: ASTDatabase):
        file = suppressKeywords("file")
        ld, rd = suppressLiterals(*FileNamespace.delimeters)

        def action(x):
            return db.addFileNamespace(FileNamespace(str(x[0]), x[1:]))

        fileNamespace = parseElement(file + P.identifier + ld + Parser.createTopDecl() + rd, action)
        return pp.OneOrMore(fileNamespace).ignore(pp.cStyleComment)

    @staticmethod
    def parse(filename, db: ASTDatabase):
        pp._enable_all_warnings()
        Parser.createTop(db).ignore(pp.cStyleComment).parseFile(filename, parseAll=True)


def parse(filename, db: ASTDatabase):
    Parser.parse(filename, db)
