#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Cxx.type import Type
from Cxx.type import NodeType
from Cxx.util import CxxList
from Cxx.variable import Variable
from Utility.util import formatStr, indentTxt
from Cxx.struct import Struct


class StaticNode(Type):
    def __init__(self, identifier: str):
        super().__init__(identifier)

    def parseStr(self, indentation=0):
        return str("static " + self.identifier)

    def cxx(self):
        return self.__str__()

    def typename(self) -> str:
        return str(self)


class DynamicNode(Type):
    def __init__(self, identifier: str):
        super().__init__(identifier)


class DynamicList(Type):
    def __init__(self, identifier: str):
        super().__init__(identifier)

    def parseStr(self, indentation=0):
        return str("list " + self.identifier)


class StaticList(Type):
    def __init__(self, identifier: str):
        super().__init__(identifier)

    def parseStr(self, indentation=0):
        return str("static list " + self.identifier)


class ChildNode(Variable):
    pass

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
    
    def typename(self):
        return NodeType(self.identifier)

    def hasClassOf(self):
        return self.classOf and len(self.classOf)

    def headerParseStr(self):
        if self.classOf:
            return "node {}<{}>".format(self.identifier, self.classOf)
        return "node {}".format(self.identifier)

    def extraMembers(self):
        return self.children.parseStr(2)

    def setBody(self, children=None, **kwargs):
        if children:
            self.children = ChildrenList(children)
        Struct.setBody(self, **kwargs)

    @staticmethod
    def createFromParse(identifier, classOf=None, **kwargs):
        node = Node(identifier=identifier, classOf=classOf)
        node.setBody(**kwargs)
        return node
