#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

import textwrap
from Lang.type import NodeType


class Struct:
    def __init__(self, identifier, parents=None, members=None, make=None, extra=None, isDefinition=False) -> None:
        self.identifier = identifier
        self.parents = parents
        self.members = members
        self.make = make
        self.extra = extra
        self.isDefinition = isDefinition

    def __str__(self) -> str:
        return self.getParseStr()

    __repr__ = __str__

    def hasParents(self):
        return self.parents and len(self.parents)

    def hasMembers(self):
        return self.members and len(self.members)

    def hasMake(self):
        return self.make and len(self.make)

    def hasExtra(self):
        return self.extra and len(self.extra)

    def headerParseStr(self):
        if self.isDefinition:
            return "struct {}".format(self.identifier)
        return "struct {};".format(self.identifier)

    def parentsParseStr(self):
        if not self.hasParents():
            return ""
        return "  parents: [{}];\n".format(", ".join(map(str, self.parents)))

    def membersParseStr(self):
        if not self.hasMembers():
            return ""
        tmp = textwrap.indent(",\n".join(map(str, self.members)), "    ", lambda x: True)
        return "  members: [\n{}  ];\n".format(tmp)

    def makeParseStr(self):
        if not self.hasMake():
            return ""
        tmp = textwrap.indent(",\n".join(map(str, self.make)), "    ", lambda x: True)
        return "  make: [\n{}  ];\n".format(tmp)

    def extraParseStr(self):
        if not self.hasMake():
            return ""
        tmp = textwrap.indent(textwrap.dedent(self.extra), "    ", lambda x: True)
        return "  :{\n{}  }:\n".format(tmp)

    def getParseStr(self) -> str:
        if self.isDefinition:
            body = ""
            body += self.parentsParseStr()
            body += self.membersParseStr()
            body += self.makeParseStr()
            body += self.extraParseStr()
            return self.headerParseStr() + " {{\n{}}};".format(body)
        return self.headerParseStr()

    def setBodyFromParse(self, parseElements):
        print(parseElements)

    @staticmethod
    def createFromParse(parseElements):
        identifier = parseElements[0]
        node = Struct(identifier=identifier)
        if len(parseElements) > 1:
            node.setBodyFromParse(parseElements[1])
        return node


class Node(Struct):
    def __init__(self, identifier, classOf=None, parents=None, members=None, make=None, extra=None) -> None:
        super().__init__(identifier, parents=parents, members=members, make=make, extra=extra, isDefinition=True)
        self.classOf = classOf

    def hasClassOf(self):
        return self.classOf and len(self.classOf)

    def headerParseStr(self):
        if self.classOf:
            return "node {}<{}>".format(self.identifier, self.classOf)
        return "node {}".format(self.identifier)

    @staticmethod
    def createFromParse(parseElements):
        identifier = parseElements[0]
        classOf = NodeType(parseElements[1][0]) if len(parseElements[1]) else None
        node = Node(identifier=identifier, classOf=classOf)
        if len(parseElements) > 2:
            node.setBodyFromParse(parseElements[2])
        return node
