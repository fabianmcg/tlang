#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

import pathlib
from Lang.db import LangDB
from Lang.node import Node
from Utility.type import EnumType, Vector, UniquePtr
from Utility.variable import Variable


def generateConstructors(node: Node):
    src = "{0:}() = default;\nvirtual ~{0:}() = default;\n{0:}({0:}&&) = default;\n{0:}(const {0:}&) = delete;"
    src = src.format(node.typename())
    if len(node.members.values()) > 0:
        variables = []
        initializers = []
        c = 0
        for var in node.members.values():
            T = var.T
            if isinstance(T, (Vector, UniquePtr)):
                variables.append("{}&& _{}".format(T, c))
                initializers.append("__{}(std::move(_{}))".format(var.identifier, c))
            else:
                variables.append("const {}& _{}".format(T, c))
                initializers.append("__{}(_{})".format(var.identifier, c))
            c += 1
        src += "{}({}): {}{{}}".format(node.typename(), " , ".join(variables), " , ".join(initializers))
    src += "{0:}& operator=({0:}&&) = default;\n{0:}& operator=(const {0:}&) = delete;".format(node.typename())
    return src


def generateMembers(var: Variable, node: Node):
    ID = var.identifier
    T = var.T
    variableName = "__" + ID
    variable = "{} {} = {{}};\n".format(T, variableName)
    accessors = ""
    kindMethods = ""
    if isinstance(T, Vector):
        accessors += "\n" + "{}& get{}() {{\n return {};\n}}".format(T, ID.capitalize(), variableName)
        accessors += "\n" + "const {}& get{}() const {{\n return {};\n}}".format(T, ID.capitalize(), variableName)
        accessors += "\n" + "void set{}({}&& x) {{\n {} = std::move(x);\n}}".format(ID.capitalize(), T, variableName)
        TT = T.underlying()
        if isinstance(TT, (Vector, UniquePtr)):
            accessors += "\n" + "void append{}({}&& x) {{\n {}.push_back(std::move(x));\n}}".format(
                ID.capitalize(), TT, variableName
            )
        else:
            accessors += "\n" + "void append{}(const {}& x) {{\n {}.push_back(x);\n}}".format(
                ID.capitalize(), TT, variableName
            )
    elif isinstance(T, UniquePtr):
        accessors += "\n" + "{}* get{}() const {{\n return {}.get();\n}}".format(
            T.underlying().typename(), ID.capitalize(), variableName
        )
        accessors += "\n" + "void set{}({}&& x) {{\n {} = std::move(x);\n}}".format(ID.capitalize(), T, variableName)
    elif isinstance(T, EnumType):
        enum = node.types[T.typename()]
        for k in enum.enum:
            kindMethods += "\n" + "bool is{}() const {{\n return {} == {}; }}".format(k.name, ID, k.name)
        accessors += "\n" + "{}_t& get{}() {{\n return {};\n}}".format(T, ID.capitalize(), variableName)
        accessors += "\n" + "const {}_t get{}() const {{\n return {};\n}}".format(T, ID.capitalize(), variableName)
        accessors += "\n" + "void set{}({}_t x) {{\n {} = x;\n}}".format(ID.capitalize(), T, variableName)
    else:
        accessors += "\n" + "{}& get{}() {{\n return {};\n}}".format(T, ID.capitalize(), variableName)
        accessors += "\n" + "const {}& get{}() const {{\n return {};\n}}".format(T, ID.capitalize(), variableName)
        accessors += "\n" + "void set{}(const {}& x) {{\n {} = x;\n}}".format(ID.capitalize(), T, variableName)
    return variable, accessors, kindMethods


def generateLocationMethods():
    src = "SourceLocation getBeginLoc(SourceLocation& loc) const { return __range.begin; }"
    src += "SourceLocation getEndLoc(SourceLocation& loc) const { return __range.end; }"
    src += "void setBeginLoc(SourceLocation& loc) { __range.begin = loc; }"
    src += "void setEndLoc(SourceLocation& loc) { __range.end = loc; }"
    src += "SourceRange getSourceRange() const { return __range; }"
    src += "void setSourceRange(SourceRange& range) { __range = range; }"
    return src


def generateClass(node: Node):
    classOf = node.classOf + "Class"
    src = "class {} {{\n{}}};"
    body = "public:\nstatic constexpr {0:} kind = {0:}::{1:};\n".format(classOf, node.typename())
    accessors = ""
    methods = ""
    variables = ""
    header = node.typename()
    if len(node.parents.parents):
        header += " : public {}".format(node.parents.parents[0])
    for parent in node.parents.parents[1:]:
        header += ", {}".format(parent)
    for enum in node.types.values():
        enumSrc = "enum {} {{ {} }};"
        b = ""
        for k in enum.enum:
            b += "{} = {},\n".format(k.name, k.value)
        body += enumSrc.format(enum.identifier, b) + "using {0:}_t = {0:};".format(enum.identifier)
    body += generateConstructors(node)
    body += "\n" + "virtual {0:} classOf() const {{ return kind; }};".format(node.classOf + "Class", node.typename())
    if node.hasLocation:
        body += generateLocationMethods()
        if len(variables) == 0:
            variables = "protected:\nExtent __extent = {};\n"
    for var in node.members.values():
        if len(variables) == 0:
            variables = "protected:\n"
        v, a, k = generateMembers(var, node)
        variables += v
        accessors += a
        methods += k

    body += accessors + methods + variables
    return src.format(header, body)


def generateEnum(classOf, nodes):
    ID = classOf + "Class"
    enum = ""
    to_string = ""
    for node in nodes.values():
        enum += node.typename() + ",\n"
        to_string += 'case {0:}::{1:}:\n return "{1:}";'.format(ID, node.typename())
    src = "enum class {} {{\n{}}};".format(ID, enum)
    src += 'std::string to_string({0:} kind) {{switch(kind){{{1:}default: return "Unknown {0:}";}}}}'.format(
        ID, to_string
    )
    return src


def generateAstNodes(grammar: LangDB, outdir: str, inputDir="Templates"):
    for classOf, nodes in grammar.nodesByClass.items():
        src = generateEnum(classOf[1], nodes)
        fwd = ""
        structs = ""
        for node in nodes.values():
            fwd += "class {};\n".format(node.typename())
            structs += generateClass(node) + "\n\n"
        src += fwd + "\n" + structs
        src = "#ifndef __AST_{0:}__\n#define __AST_{0:}__\n#include <macros.hh>\n\nnamespace _astnp_ {{\n{1:}}}\n#endif".format(
            classOf[1].upper(), src
        )
        with open(pathlib.Path(outdir, classOf[0].lower() + ".hh"), "w") as file:
            print(src, file=file)
        from Utility.format import format

        format(pathlib.Path(outdir, classOf[0].lower() + ".hh"))
