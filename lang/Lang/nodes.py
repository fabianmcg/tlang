#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.format import formatStr, indentTxt
from Utility.util import getCxx, getParseStr


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
        return indentTxt("{}\n{}\n{}\n".format(CodeSection.begin, tmp, CodeSection.end), indentation)

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
        return indentTxt("{}\n{}\n{}\n".format(HeaderSection.begin, tmp, HeaderSection.end), indentation)


class FileNamespace:
    begin = "{"
    end = "};"
    delimeters = [begin, end]

    def __init__(self, identifier, declarations) -> None:
        self.identifier = identifier
        self.declarations = declarations

    def __str__(self) -> str:
        return self.parseStr()

    __repr__ = __str__

    def hasDecls(self):
        return self.declarations and len(self.declarations)

    def parseStr(self, indentation=0):
        declarations = map(lambda x: getParseStr(x, indentation + 2), self.declarations) if self.declarations else []
        return "file {} {}\n{}\n{}".format(
            self.identifier, FileNamespace.begin, "\n".join(declarations), FileNamespace.end
        )

    def cxx(self):
        declarations = map(getCxx, self.declarations) if self.declarations else []
        return "\n".join(declarations)


class ASTDatabase:
    def __init__(self) -> None:
        self.nodes = {}
        self.fileNamespaces = {}

    def addFileNamespace(self, namespace: FileNamespace):
        self.fileNamespaces[namespace.identifier] = namespace

    def __str__(self) -> str:
        return self.parseStr()

    __repr__ = __str__

    def parseStr(self, indentation=0):
        return "\n".join(map(getParseStr, self.fileNamespaces.values()))
