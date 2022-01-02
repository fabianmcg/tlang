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
from Utility.util import formatStr, generateSectionComment, getCxx, indentTxt
from Cxx.struct import Struct


class StaticNode(Type):
    def __init__(self, identifier: str):
        super().__init__(identifier)

    def parseStr(self, indentation=0):
        return str("static " + self.identifier)

    def cxx(self, offset):
        return "child_node<child_kind::static_node, {}, {}>".format(self.identifier, offset)

    def typename(self) -> str:
        return str(self)


class DynamicNode(Type):
    def __init__(self, identifier: str):
        super().__init__(identifier)

    def cxx(self, offset):
        return "child_node<child_kind::dynamic_node, {}, {}>".format(self.identifier, offset)


class StaticList(Type):
    def __init__(self, identifier: str):
        super().__init__(identifier)

    def cxx(self, offset):
        return "child_node<child_kind::static_list, {}, {}>".format(self.identifier, offset)

    def parseStr(self, indentation=0):
        return str("static list " + self.identifier)


class DynamicList(Type):
    def __init__(self, identifier: str):
        super().__init__(identifier)

    def cxx(self, offset):
        return "child_node<child_kind::dynamic_list, {}, {}>".format(self.identifier, offset)

    def parseStr(self, indentation=0):
        return str("list " + self.identifier)


class ChildNode(Variable):
    def cxx(self):
        return self.T.cxx(self.identifier + "Offset")

    def getCxx(self):
        src = (
            "template <access_kind kind = access_kind::pointer>"
            + "{2:} child_return_t<{0:}Offset, kind> get{1:}({3:}) {2:} "
            + "{{ return __children.template get<{0:}Offset, kind>({4:});}}\n"
        )
        const = src.format(self.identifier, self.identifier.capitalize(), "", "", "")
        no_const = src.format(self.identifier, self.identifier.capitalize(), "const", "", "")
        if isinstance(self.T, (StaticList, DynamicList)):
            src = (
                "template <access_kind kind = access_kind::pointer>"
                + "{2:} auto get{1:}({3:}) {2:} "
                + "{{ return __children.template get<{0:}Offset, kind>({4:});}}\n"
            )
            const += src.format(self.identifier, self.identifier.capitalize(), "", "size_t i", "i")
            no_const += src.format(self.identifier, self.identifier.capitalize(), "const", "size_t i", "i")
        return const + no_const

    def hasCxx(self):
        return "bool has{0:}() const {{ return __children.template has<{1:}Offset>();}}\n".format(
            self.identifier.capitalize(), self.identifier
        )


class ChildrenList(CxxList):
    def parseStr(self, indentation=0):
        if self.empty():
            return ""
        tmp = indentTxt(",\n".join(map(str, self)), 2)
        return indentTxt("children: [\n{}\n];\n".format(tmp), indentation)

    def cxx(self):
        return ""


class Node(Struct):
    def __init__(self, identifier, classOf=None, noDefault=False) -> None:
        super().__init__(identifier, True, noDefault)
        self.classOf = classOf
        self.children = ChildrenList()

    def hasClassOf(self):
        return self.classOf and len(self.classOf)

    def typename(self):
        return NodeType(self.identifier)

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
    def createFromParse(identifier, header=None, **kwargs):
        classOf = header[0] if header[0] else None
        noDefault = header[1] == "noDefault"
        node = Node(identifier=identifier, classOf=classOf, noDefault=noDefault)
        node.setBody(**kwargs)
        return node

    def cxxNodeConstructor(self, const: bool, skipASTNode=True):
        src = "template <typename...Args> {}(".format(self.identifier)
        i = ""
        addComma = False
        for parent in self.parents:
            if skipASTNode and parent == "ASTNode":
                continue
            src += "," if addComma else ""
            i += "," if addComma else ""
            src += "{}&& __{}".format(parent, str(parent).lower())
            i += "{}(std::move(__{}))".format(parent, str(parent).lower())
            addComma = True
        for member in self.members:
            src += "," if addComma else ""
            i += "," if addComma else ""
            if const:
                src += "const {}& _{}".format(member.T, str(member.identifier).lower())
                i += "{}(_{})".format(member.varName(), str(member.identifier).lower())
            else:
                src += "{}&& _{}".format(member.T, str(member.identifier).lower())
                i += "{}(std::move(_{}))".format(member.varName(), str(member.identifier).lower())
            addComma = True
        i += "," if addComma else ""
        i += "__children(std::forward<Args>(args)...)"
        src = src + "{}Args&&...args) {} {{}}".format(", " if addComma else "", ": " + i if len(i) else "")
        return src

    def cxxSpecialConstructor(self, init=False):
        tmp = ""
        if len(self.members) or "ASTNode" in self.parents:
            tmp = self.cxxNodeConstructor(True, False)
        return self.cxxNodeConstructor(False, True) + tmp

    def cxxPostHeader(self):
        enum = ", ".join(map(lambda x: x.identifier + "Offset", self.children))
        src = "enum {{{} endOffset}};".format(enum + ("," if len(enum) else ""))
        src += "using children_t = children_container<{}>;\n".format(", ".join(map(getCxx, self.children)))
        src += "template <int offset, access_kind kind> using child_return_t = typename children_t::template return_t<offset, kind>;\n"
        src += 'static_assert(children_t::size == endOffset, "Incongruent number of children.");\n'
        src += "using node_kind_t = NodeClass;\nstatic constexpr node_kind_t kind = node_kind_t::{};\n".format(
            self.identifier
        )
        src += "virtual node_kind_t classOf() const { return kind; }\n"
        return src

    def cxxPreMembers(self):
        return generateSectionComment("Children opetors") + (
            "children_t* operator->() { return &__children; }\n"
            "const children_t* operator->() const { return &__children; }\n"
            "children_t& operator*() { return __children; }\n"
            "const children_t& operator*() const { return __children; }\n"
        )

    def cxxClone(self):
        parents = ", ".join(["{}::clone()".format(parent) for parent in self.parents]) + ", "
        members = ", ".join(map(lambda x: x.varName(), self.members))
        members += ", " if len(members) else ""
        src = "return {}({}{}__children.clone());".format(self.identifier, parents, members)
        tmp = "virtual std::unique_ptr<ASTNode> clonePtr() const {{ return std::make_unique<{}>(clone()); }}".format(
            self.identifier
        )
        return "{} clone() const {{\n{}}}\n{}\n".format(self.identifier, src, tmp)

    def cxxPostMembers(self):
        src = self.addComment("Children accessors", "\n".join(map(lambda x: x.getCxx(), self.children)))
        src += self.addComment("Has methods", "\n".join(map(lambda x: x.hasCxx(), self.children)))
        src += self.addComment("Clone", self.cxxClone())
        return src

    def cxxProtectedSectionBody(self):
        src = Struct.cxxProtectedSectionBody(self)
        src += "children_t __children{};"
        return src
