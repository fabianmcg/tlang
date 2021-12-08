#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Cxx.type import EnumType, StructType, Type
from Cxx.util import CxxList
from Utility.util import formatStr, indentTxt


class CodeSection:
    begin = ":{"
    end = "}:"
    delimeters = [begin, end]

    def __init__(self, code: str):
        self.code = formatStr(code.strip())

    def __str__(self) -> str:
        return self.parseStr()

    def __repr__(self) -> str:
        return str(self)

    def empty(self):
        return len(self.code) == 0

    def notEmpty(self):
        return not self.empty()

    def parseStr(self, indentation=0):
        if self.empty():
            return ""
        tmp = indentTxt(self.code, 2)
        return indentTxt("{}\n{}\n{}".format(CodeSection.begin, tmp, CodeSection.end), indentation)

    def cxx(self):
        return self.code


class HeaderSection(CodeSection):
    begin = ".{"
    end = "}."
    delimeters = [begin, end]

    def parseStr(self, indentation=0):
        if self.empty():
            return ""
        tmp = indentTxt(self.code, 2)
        return indentTxt("{}\n{}\n{}".format(HeaderSection.begin, tmp, HeaderSection.end), indentation)


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


class EnumElement:
    def __init__(self, identifier, value=None) -> None:
        self.identifier = identifier
        self.value = value

    def __str__(self) -> str:
        return self.identifier + (" = " + self.value if self.value else "")

    def __repr__(self) -> str:
        return str(self)


class Enum:
    def __init__(self, identifier, isClass, values) -> None:
        self.identifier = identifier
        self.isClass = isClass
        self.values = values

    def __str__(self) -> str:
        return self.parseStr()

    def __repr__(self) -> str:
        return str(self)

    def empty(self):
        return len(self.values) == 0

    def notEmpty(self):
        return not self.empty()

    def parseStr(self, indentation=0):
        if self.empty():
            return ""
        tmp = indentTxt(",\n".join(map(str, self.values)), 2)
        isClass = "class " if self.isClass else ""
        return indentTxt("enum {}{} {{\n{}\n}};".format(isClass, self.identifier, tmp), indentation)
    
    def typename(self):
        return EnumType(self.identifier)


class EnumList(CxxList):
    def parseStr(self, indentation=0):
        if self.empty():
            return ""
        tmp = indentTxt("\n".join(map(str, self)), 0)
        return indentTxt("{}\n".format(tmp), indentation)

    def cxx(self):
        return ""


class Struct:
    def __init__(self, identifier, isDefinition=False) -> None:
        self.identifier = identifier
        self.parents = ParentList()
        self.members = MemberList()
        self.enums = EnumList()
        self.headerSection = HeaderSection("")
        self.epilogueSection = CodeSection("")
        self.isDefinition = isDefinition

    def __str__(self) -> str:
        return self.parseStr()

    __repr__ = __str__
    
    def typename(self):
        return StructType(self.identifier)

    def headerParseStr(self):
        if self.isDefinition:
            return "struct {}".format(self.identifier)
        return "struct {};".format(self.identifier)

    def extraMembers(self):
        return ""

    def addSpace(self, x):
        return x + "\n" if len(x) else x

    def parseStr(self, indentation=0) -> str:
        if self.isDefinition:
            body = ""
            body += self.parents.parseStr(2)
            body += self.members.parseStr(2)
            body += self.extraMembers()
            body += self.enums.parseStr(2)
            body += self.addSpace(self.headerSection.parseStr(2))
            body += self.addSpace(self.epilogueSection.parseStr(2))
            return indentTxt(self.headerParseStr() + " {{\n{}}};".format(body), indentation)
        return indentTxt(self.headerParseStr(), indentation)

    def setBody(self, parents=None, members=None, enums=None, headerSection=None, epilogueSection=None, **kwargs):
        if parents:
            self.parents = ParentList(parents)
        if members:
            self.members = MemberList(members)
        if enums:
            self.enums = EnumList(enums)
        if headerSection:
            self.headerSection = headerSection
        if epilogueSection:
            self.epilogueSection = epilogueSection

    @staticmethod
    def createFromParse(identifier, **kwargs):
        identifier = identifier
        node = Struct(identifier=identifier)
        node.setBody(**kwargs)
        return node
