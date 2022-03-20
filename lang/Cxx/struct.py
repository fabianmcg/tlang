#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Cxx.type import EnumType, StructType, Type
from Cxx.util import CxxList
from Utility.util import formatStr, generateSectionComment, getCxx, getParseStr, indentTxt


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
    def __init__(self, identifier, isClass, values, insideClass=False) -> None:
        self.identifier = identifier
        self.isClass = isClass
        self.values = values
        self.insideClass = insideClass

    def __str__(self) -> str:
        return self.parseStr()

    def __repr__(self) -> str:
        return str(self)

    def empty(self):
        return len(self.values) == 0

    def notEmpty(self):
        return not self.empty()

    def typename(self):
        return EnumType(self.identifier)

    def parseStr(self, indentation=0):
        if self.empty():
            return ""
        tmp = indentTxt(",\n".join(map(str, self.values)), 2)
        isClass = "class " if self.isClass else ""
        return indentTxt("enum {}{} {{\n{}\n}};".format(isClass, self.identifier, tmp), indentation)

    def cxxOperators(self):
        if self.insideClass or not self.isClass:
            return ""
        op = "inline {0:} operator {1:}({0:} x, {0:} y) {{ return static_cast<{0:}>(static_cast<int>(x) {1:} static_cast<int>(y)); }}"
        return op.format(self.identifier, "|") + op.format(self.identifier, "&")

    def cxx(self):
        ops = self.cxxOperators()
        tmp = ""
        if not self.isClass:
            tmp = "\nusing {0:}_t = {0:};".format(self.identifier)
        return self.parseStr() + tmp + ("\n" + ops if len(ops) else "")

    def isCxx(self, memberName):
        isClass = "{}::".format(self.identifier) if self.isClass else ""
        method = "bool is{}() const {{ return {} == {}{}; }}\n"
        src = ""
        for value in self.values:
            src += method.format(value.identifier.capitalize(), memberName, isClass, value.identifier)
        return src


class EnumList(CxxList):
    def parseStr(self, indentation=0):
        if self.empty():
            return ""
        tmp = indentTxt("\n".join(map(str, self)), 0)
        return indentTxt("{}\n".format(tmp), indentation)

    def cxx(self):
        return ""


class Struct:
    def __init__(self, identifier, isDefinition=False, noDefault=False) -> None:
        self.identifier = identifier
        self.parents = ParentList()
        self.members = MemberList()
        self.enums = EnumList()
        self.headerSection = HeaderSection("")
        self.epilogueSection = CodeSection("")
        self.isDefinition = isDefinition
        self.noDefault = noDefault

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

    def addComment(self, comment, x):
        if x:
            return generateSectionComment(comment) + x
        return ""

    def cxxHeader(self):
        src = "public:\n" + generateSectionComment("Header")
        src += "using parents_t = parent_container<{}>;\n".format(", ".join(map(str, self.parents)))
        enums = ""
        if self.enums.notEmpty():
            enums = "\n".join(map(getCxx, self.enums))
        return src + self.cxxPostHeader() + enums

    def cxxPostHeader(self):
        return ""

    def cxxSpecialConstructor(self, init=False):
        src = "{}(".format(self.identifier)
        i = ""
        addComma = False
        for parent in self.parents:
            src += "," if addComma else ""
            i += "," if addComma else ""
            src += "{}&& _{}".format(parent, str(parent).lower())
            i += "{}(std::move(_{}))".format(parent, str(parent).lower())
            addComma = True
        for member in self.members:
            src += "," if addComma else ""
            i += "," if addComma else ""
            src += "{}&& _{}".format(member.T, str(member.identifier).lower())
            i += "{}(std::move(_{}))".format(member.varName(), str(member.identifier).lower())
            addComma = True
        return src + ") {} {{}}".format(": " + i if len(i) else "")

    def cxxConstructorsAndOperators(self):
        src = "" if self.noDefault else "{0:}() = default;\n".format(self.identifier)
        if self.parents.notEmpty():
            src += "virtual ~{0:}() = default;".format(self.identifier)
        else:
            src += "\n ~{0:}() = default;".format(self.identifier)
        src += "\n{0:}({0:}&&) = default;\n{0:}(const {0:}&) = default;".format(self.identifier)
        src += self.cxxSpecialConstructor()
        src += "{0:}& operator=({0:}&&) = default;\n{0:}& operator=(const {0:}&) = default;\n".format(self.identifier)
        return src

    def cxxPreMembers(self):
        return ""

    def cxxMembers(self):
        src = "\n".join(map(lambda x: x.getCxx(), self.members))
        if len(src):
            src += "\n"
        return self.addComment("Member modifiers", src)

    def cxxPostMembers(self):
        return ""

    def cxxEnumIsMethods(self):
        src = ""
        enums = {e.identifier: e for e in self.enums}
        for member in self.members:
            if str(member.T) in enums:
                src += enums[str(member.T)].isCxx(member.varName())
        return self.addComment("Is methods", src)

    def cxxProtectedSectionBody(self):
        return "\n".join(map(lambda x: getCxx(x) + "{};", self.members))

    def cxxProtectedSection(self):
        src = self.cxxProtectedSectionBody()
        if len(src):
            src = "protected:\n" + src
        return src

    def cxxBody(self):
        tmp = self.headerSection.cxx()
        src = self.cxxHeader() + (tmp + "\n" if len(tmp) else "")
        src += self.addComment("Constructors & operators", self.cxxConstructorsAndOperators())
        src += self.cxxPreMembers()
        src += self.cxxMembers()
        src += self.cxxPostMembers()
        src += self.cxxEnumIsMethods()
        src += self.addComment("Epilogue", self.epilogueSection.cxx() + self.cxxProtectedSection())
        return src

    def cxx(self):
        src = "class {}{}{{{}}};"
        parents = ",".join(map(lambda x: "public " + x, self.parents))
        parents = ": " + parents if len(parents) else ""
        return src.format(self.identifier, parents, self.cxxBody())
    
    def td(self):
        return ""