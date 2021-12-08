#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Cxx.type import Type
from Cxx.type import NodeType
from Lang.nodes import CodeSection, HeaderSection
from Utility.format import indentTxt


class CxxList(list):
    def __str__(self) -> str:
        return self.parseStr()

    def __repr__(self) -> str:
        return str(self)

    def empty(self):
        return len(self) == 0

    def notEmpty(self):
        return not self.empty()

    def parseStr(self, indentation=0):
        return ""


class ParentList(CxxList):
    def parseStr(self, indentation=0):
        if self.empty():
            return ""
        return indentTxt("parents: [{}];\n".format(", ".join(map(str, self))), indentation)

    def cxx(self):
        return ", ".join(map(str, self))


class MemberList(CxxList):
    def parseStr(self, indentation=0):
        if self.empty():
            return ""
        tmp = indentTxt(",\n".join(map(str, self)), 2)
        return indentTxt("members: [\n{}\n];\n".format(tmp), indentation)

    def cxx(self):
        return ""


class Struct:
    def __init__(self, identifier, isDefinition=False) -> None:
        self.identifier = identifier
        self.parents = ParentList()
        self.members = MemberList()
        self.headerSection = HeaderSection("")
        self.epilogueSection = CodeSection("")
        self.isDefinition = isDefinition

    def __str__(self) -> str:
        return self.parseStr()

    __repr__ = __str__

    def headerParseStr(self):
        if self.isDefinition:
            return "struct {}".format(self.identifier)
        return "struct {};".format(self.identifier)

    def extraMembers(self):
        return ""

    def parseStr(self, indentation=0) -> str:
        if self.isDefinition:
            body = ""
            body += self.parents.parseStr(2)
            body += self.members.parseStr(2)
            body += self.extraMembers()
            body += self.headerSection.parseStr(2)
            body += self.epilogueSection.parseStr(2)
            return indentTxt(self.headerParseStr() + " {{\n{}}};".format(body), indentation)
        return indentTxt(self.headerParseStr(), indentation)

    def setBodyFromParse(self, parseElements):
        self.parents = ParentList(parseElements[0])
        self.members = MemberList(parseElements[1])
        if isinstance(parseElements[2], HeaderSection):
            self.headerSection = parseElements[2]
        if isinstance(parseElements[3], CodeSection):
            self.epilogueSection = parseElements[3]

    @staticmethod
    def createFromParse(parseElements):
        identifier = parseElements[0]
        node = Struct(identifier=identifier)
        if len(parseElements) > 1:
            node.setBodyFromParse(parseElements[1])
        return node


class ChildrenList(CxxList):
    def parseStr(self, indentation=0):
        if self.empty():
            return ""
        tmp = indentTxt(",\n".join(map(str, self)), 2)
        return indentTxt("children: [\n{}\n];\n".format(tmp), indentation)

    def cxx(self):
        return ""


class Node(Struct):
    def __init__(self, identifier, classOf=None) -> None:
        super().__init__(identifier, True)
        self.classOf = classOf
        self.children = ChildrenList()

    def hasClassOf(self):
        return self.classOf and len(self.classOf)

    def headerParseStr(self):
        if self.classOf:
            return "node {}<{}>".format(self.identifier, self.classOf)
        return "node {}".format(self.identifier)

    def extraMembers(self):
        return self.children.parseStr(2)

    def setBodyFromParse(self, parseElements):
        self.parents = ParentList(parseElements[0])
        self.members = MemberList(parseElements[1])
        self.children = ChildrenList(parseElements[2])
        if isinstance(parseElements[3], HeaderSection):
            self.headerSection = parseElements[3]
        if isinstance(parseElements[4], CodeSection):
            self.epilogueSection = parseElements[4]

    @staticmethod
    def createFromParse(parseElements):
        identifier = parseElements[0]
        classOf = NodeType(parseElements[1][0]) if len(parseElements[1]) else None
        node = Node(identifier=identifier, classOf=classOf)
        if len(parseElements) > 2:
            node.setBodyFromParse(parseElements[2])
        return node
