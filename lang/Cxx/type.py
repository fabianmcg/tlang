#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.util import getCxx


class Type:
    def __init__(self, identifier: str):
        self.identifier = identifier

    def __str__(self) -> str:
        return self.parseStr()

    def __repr__(self) -> str:
        return str(self)

    @staticmethod
    def make(x):
        return x if isinstance(x, Type) else Type(x)

    @staticmethod
    def check(x):
        if isinstance(x, Type):
            return x
        raise (Exception("{}: is not a type".format(x)))

    def isTemplate(self):
        return False

    def cxx(self):
        return str(self)

    def typename(self) -> str:
        return str(self)

    def parseStr(self):
        return str(self.identifier)


class UnresolvedType(Type):
    def __init__(self, identifier):
        super().__init__(identifier)


class AutoType(Type):
    def __init__(self):
        super().__init__("auto")


class BoolType(Type):
    def __init__(self):
        super().__init__("bool")


class IntType(Type):
    def __init__(self):
        super().__init__("int")


class SizeType(Type):
    def __init__(self):
        super().__init__("size_t")


class FloatType(Type):
    def __init__(self):
        super().__init__("float")


class VoidType(Type):
    def __init__(self):
        super().__init__("void")


class StringType(Type):
    def __init__(self):
        super().__init__("string")

    def cxx(self):
        return "std::string"


class StructType(Type):
    def __init__(self, identifier):
        super().__init__(identifier)


class NodeType(Type):
    def __init__(self, identifier):
        super().__init__(identifier)


class EnumType(Type):
    def __init__(self, identifier):
        super().__init__(identifier)


class TemplateType(Type):
    def __init__(self, identifier: str, *templateArgs):
        super().__init__(identifier)
        self.templateArgs = list(templateArgs)

    def __str__(self) -> str:
        return self.parseStr()

    def isTemplate(self):
        return True

    def typename(self):
        return self.identifier

    def underlying(self):
        return self.templateArgs[0] if len(self.templateArgs) else None

    def cxxTypename(self):
        return self.identifier

    def cxxArgStr(self):
        return ", ".join(map(getCxx, self.templateArgs))

    def argStr(self):
        return ", ".join(map(str, self.templateArgs))

    def parseStr(self):
        return "{}<{}>".format(self.identifier, self.argStr())

    def cxx(self) -> str:
        return "{}<{}>".format(self.cxxTypename(), self.cxxArgStr())


class UnresolvedTemplateType(TemplateType):
    def __init__(self, identifier: str, *templateArgs):
        super().__init__(identifier, *templateArgs)


class PointerType(TemplateType):
    def __init__(self, underlying):
        super().__init__("ptr", Type.check(underlying))


class ReferenceType(TemplateType):
    def __init__(self, underlying):
        super().__init__("ref", Type.check(underlying))


class VectorType(TemplateType):
    def __init__(self, underlying):
        super().__init__("vector", Type.check(underlying))

    def cxxTypename(self):
        return "std::vector"


class UniquePtrType(TemplateType):
    def __init__(self, underlying):
        super().__init__("unique", Type.check(underlying))

    def cxxTypename(self):
        return "std::unique_ptr"


class OptionalType(TemplateType):
    def __init__(self, underlying):
        super().__init__("opt", Type.check(underlying))

    def cxxTypename(self):
        return "std::optional"


def getIdentifier(t):
    if issubclass(t, TemplateType):
        return t(IntType()).typename()
    return t().typename()


class TypeHelper:
    baseTypeList = [
        AutoType,
        BoolType,
        IntType,
        SizeType,
        FloatType,
        VoidType,
        StringType,
    ]
    templateTypeList = [
        PointerType,
        ReferenceType,
        VectorType,
        UniquePtrType,
        OptionalType,
    ]
    typeList = baseTypeList + templateTypeList
    typeDict = {getIdentifier(t): t for t in typeList}
    baseTypeDict = {getIdentifier(t): t for t in baseTypeList}
    templateTypeDict = {getIdentifier(t): t for t in templateTypeList}

    @staticmethod
    def getType(identifier, isTemplate=False):
        if identifier in TypeHelper.typeDict:
            return TypeHelper.typeDict[identifier]

        def unresolved(*args):
            return UnresolvedTemplateType(identifier, *args) if isTemplate else UnresolvedType(identifier)

        return unresolved

    @staticmethod
    def getBaseTypenames():
        return list(TypeHelper.baseTypeDict.keys())

    @staticmethod
    def getTemplateTypenames():
        return list(TypeHelper.templateTypeDict.keys())
