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
    def __init__(self, identifier, classOf=None) -> None:
        super().__init__(identifier, True)
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
    def createFromParse(identifier, classOf=None, **kwargs):
        node = Node(identifier=identifier, classOf=classOf)
        node.setBody(**kwargs)
        return node

    def cxxPostHeader(self):
        enum = ", ".join(map(lambda x: x.identifier + "Offset", self.children))
        src = "enum {{{} endOffset}};".format(enum + ("," if len(enum) else ""))
        src += "using children_t = children_container<{}>;\n".format(", ".join(map(getCxx, self.children)))
        src += "template <int offset, access_kind kind> using child_return_t = typename children_t::template return_t<offset, kind>;\n"
        src += 'static_assert(children_t::size == endOffset, "Incongruent number of children.");\n'
        src += "using node_kind_t = NodeClass;\nstatic constexpr node_kind_t kind = node_kind_t::{};\n".format(
            self.identifier
        )
        return src + "virtual node_kind_t classOf() const { return kind; }\n"

    def cxxPreMembers(self):
        return generateSectionComment("Children opetors") + (
            "children_t* operator->() { return &__children; }\n"
            "const children_t* operator->() const { return &__children; }\n"
            "children_t& operator*() { return __children; }\n"
            "const children_t& operator*() const { return __children; }\n"
        )

    def cxxPostMembers(self):
        src = self.addComment("Children accessors", "\n".join(map(lambda x: x.getCxx(), self.children)))
        src += self.addComment("Has methods", "\n".join(map(lambda x: x.hasCxx(), self.children)))
        return src

    def cxxProtectedSectionBody(self):
        src = Struct.cxxProtectedSectionBody(self)
        src += "children_t __children{};"
        return src
