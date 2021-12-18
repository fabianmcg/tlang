#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from AST.astNode import Node
from Utility.util import (
    getCxx,
    getJinjaTemplate,
    getParseStr,
    formatStr,
    format,
    jinjaTemplate,
    pathJoin,
    printToFile,
)
from Cxx.struct import HeaderSection, Struct


class FileNamespace:
    begin = "{"
    end = "};"
    delimeters = [begin, end]

    def __init__(self, identifier, declarations: list) -> None:
        self.identifier = identifier
        self.declarations = declarations

    def __str__(self) -> str:
        return self.parseStr()

    __repr__ = __str__

    def isMain(self):
        return self.identifier == "main"

    def getId(self):
        return "ASTCommon" if self.isMain() else self.identifier

    def hasDecls(self):
        return self.declarations and len(self.declarations)

    def parseStr(self, indentation=0):
        declarations = map(lambda x: getParseStr(x, indentation + 2), self.declarations) if self.declarations else []
        return "file {} {}\n{}\n{}".format(
            self.identifier, FileNamespace.begin, "\n".join(declarations), FileNamespace.end
        )

    def cxx(self, namespace="", postHeader=""):
        src = ""
        declarations = self.declarations.copy()
        tmp = []
        if len(declarations):
            if isinstance(declarations[0], HeaderSection):
                src = getCxx(declarations.pop(0)) + "\n"
            tmp = map(getCxx, declarations) if declarations else []
        tmp = postHeader + "\n".join(tmp)
        if tmp:
            src += tmp if namespace == "" else "namespace {} {{\n{}\n}}".format(namespace, tmp)
        return formatStr(src)


class ASTDatabase:
    def __init__(self) -> None:
        self.nodes = {}
        self.nodesByClass = {}
        self.structs = {}
        self.fileNamespaces = {}

    def __str__(self) -> str:
        return self.parseStr()

    __repr__ = __str__

    def addFileNamespace(self, namespace: FileNamespace):
        self.fileNamespaces[namespace.identifier] = namespace

    def parseStr(self, indentation=0):
        return "\n".join(map(getParseStr, self.fileNamespaces.values()))

    def consolidate(self):
        if "main" not in self.fileNamespaces:
            self.addFileNamespace(FileNamespace("main", []))
        for namespace in self.fileNamespaces.values():
            for decl in namespace.declarations:
                if isinstance(decl, Node):
                    self.nodes[decl.identifier] = decl
                    if decl.classOf not in self.nodesByClass:
                        self.nodesByClass[decl.classOf] = {}
                    self.nodesByClass[decl.classOf][decl.identifier] = decl
                elif isinstance(decl, Struct):
                    self.structs[decl.identifier] = decl

    def generateMain(self):
        enum = ""
        fwd = "\n".join(map(lambda x: "class {};".format(x), list(self.nodes.keys()) + list(self.structs.keys())))
        to_string = ""
        is_methods = ""
        for nodeClass, Class in self.nodesByClass.items():
            enum += "First{},\n".format(nodeClass)
            for k, node in Class.items():
                enum += k + ",\n"
                to_string += 'case NodeClass::{0:}:\nreturn "{0:}";'.format(k)
            enum += "Last{},\n".format(nodeClass)
            is_methods += "inline constexpr bool is{0:}(NodeClass kind) {{ return (NodeClass::First{0:} < kind) && (kind < NodeClass::Last{0:});}}".format(
                nodeClass
            )
        enum = "enum class NodeClass {{\nASTNode,\nASTNodeList,\n{}}};".format(enum)
        to_string = 'inline std::string to_string(NodeClass kind) {{switch(kind){{{}default: return "Unknown NodeClass";}}}}'.format(
            to_string
        )
        return enum + is_methods + to_string + fwd

    def generateNamespace(self, namespace: FileNamespace, templatePath):
        incSrc = ""
        libSrc = ""
        identifier = "AST_{}_HEADER".format(namespace.getId())
        if not namespace.isMain():
            incSrc = namespace.cxx("_astnp_")
            incSrc = getJinjaTemplate(
                templatePath, {"ID": identifier, "HEADER": incSrc, "INCLUDES": ['"AST/ASTNode.hh"']}
            )
        else:
            incSrc = namespace.cxx("_astnp_", self.generateMain())
            incSrc = getJinjaTemplate(templatePath, {"ID": identifier, "HEADER": incSrc, "INCLUDES": ["<string>"]})
        return incSrc, libSrc

    def generateASTNodes(self, includeOutdir, libOutdir, templatePath):
        for namespace in self.fileNamespaces.values():
            inc, lib = self.generateNamespace(namespace, templatePath)
            incName = pathJoin(includeOutdir, namespace.getId() + ".hh")
            libName = pathJoin(libOutdir, namespace.getId() + ".cc")
            if len(inc):
                printToFile(inc, incName)
            if len(lib):
                printToFile(lib, libName)

    def generateRecursiveASTVisitor(self, outputDir, inputDir):
        visit = ""
        walkup = ""
        visit_pp = ""
        walkup_pp = ""
        traverse = ""
        traverse_cases = ""
        for node in self.nodes.values():
            if isinstance(node, Node):
                visit += "bool visit{0:}({0:}* node) {{ return true; }}\n".format(node.typename())
                visit_pp += "bool visit{0:}({0:}* node, bool isFirst) {{ return true; }}\n".format(node.typename())
                walkups = ""
                for p in node.parents:
                    if p != "DeclContext":
                        walkups += "WALKUP_MACRO({1:}, {0:})\n".format(node.typename(), p)
                walkup += "bool walkUpTo{0:}({0:}* node) {{ {1:} }}\n".format(node.typename(), walkups)
                walkup_pp += "bool walkUpTo{0:}({0:}* node, bool isFirst) {{ {1:} }}\n".format(node.typename(), walkups)
                traverse += (
                    "bool traverse{0:}({0:}* node, stack_t *stack = nullptr) {{ TRAVERSE_MACRO({0:}) }}\n".format(
                        node.typename()
                    )
                )
                traverse_cases += "case NodeClass::{0:}: return derived.traverse{0:}(node->template getAsPtr<{0:}>(), stack);\n".format(
                    node.typename()
                )
        jinjaTemplate(
            pathJoin(outputDir, "RecursiveASTVisitor.hh"),
            pathJoin(inputDir, "RecursiveASTVisitor.hh.j2"),
            {
                "VISIT": visit,
                "WALK_UP": walkup,
                "VISIT_PP": visit_pp,
                "WALK_UP_PP": walkup_pp,
                "TRAVERSE_CASES": traverse_cases,
                "TRAVERSE": traverse,
            },
        )
        format(pathJoin(outputDir, "RecursiveASTVisitor.hh"))
