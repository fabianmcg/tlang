#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

import pathlib
from jinja2 import Template
from Lang.db import LangDB
from Lang.node import Dynamic, ManyDynamic, Node, Static
from Utility.type import EnumType, Vector, UniquePtr
from Utility.variable import Variable


def generateParents(node: Node):
    return (
        " : " + " , ".join(map(lambda x: "public " + str(x), node.parents.parents)) if len(node.parents.parents) else ""
    )


def generateHeader(node: Node, classOf):
    src = "using parents_t = parent_container<{}>;\n".format(", ".join(map(str, node.parents.parents)))
    src += "using node_kind_t = {0:};\nstatic constexpr node_kind_t kind = node_kind_t::{1:};\n"
    return src.format(classOf, node.typename()) + "virtual node_kind_t classOf() const { return kind; }"


def generateConstructors(node: Node):
    src = "{0:}() = default;\nvirtual ~{0:}() = default;\n{0:}({0:}&&) = default;\n{0:}(const {0:}&) = delete;"
    src = src.format(node.typename())
    # if len(node.members.values()) > 0:
    #     variables = []
    #     initializers = []
    #     c = 0
    #     for var in node.members.values():
    #         T = var.T
    #         if isinstance(T, (Vector, UniquePtr)):
    #             variables.append("{}&& _{}".format(T, c))
    #             initializers.append("__{}(std::move(_{}))".format(var.identifier, c))
    #         else:
    #             variables.append("const {}& _{}".format(T, c))
    #             initializers.append("__{}(_{})".format(var.identifier, c))
    #         c += 1
    #     src += "{}({}): {}{{}}".format(node.typename(), " , ".join(variables), " , ".join(initializers))
    src += "{0:}& operator=({0:}&&) = default;\n{0:}& operator=(const {0:}&) = delete;\n".format(node.typename())
    return src


def generateChildren(node: Node):
    header = ""
    enum = []
    children = []
    getMethods = ""
    setMethods = ""
    pushMethods = ""
    createMethods = ""
    hasMethods = ""
    for child in node.children.data:
        visit = "" if child.visit else ", false"
        enumTmp = child.identifier + "Offset"
        kind = "children_kind::"
        identifier = child.identifier.capitalize()
        if isinstance(child, (Dynamic, Static)):
            kind += "dynamic_node" if isinstance(child, Dynamic) else "static_node"
            getMethods += "{}* get{}() const {{ return __children.template get<{}>();}}\n".format(
                child.T, identifier, enumTmp
            )
            getMethods += "{}& get{}Ref() {{ return __children.template getRef<{}>();}}\n".format(
                child.T, identifier, enumTmp
            )
            getMethods += "const {}& get{}Ref() const {{ return __children.template getRef<{}>();}}\n".format(
                child.T, identifier, enumTmp
            )
        elif isinstance(child, ManyDynamic):
            kind += "dynamic_list"
            pushMethods += "template <typename T> void push{}(T&& value) {{ __children.template push<{}>(std::forward<T>(value));}}\n".format(
                identifier, enumTmp
            )
            getMethods += "{}* get{}(size_t i) const {{ return __children.template get<{}>(i);}}\n".format(
                child.T, identifier, enumTmp
            )
            getMethods += "{}& get{}Ref(size_t i) {{ return __children.template getRef<{}>(i);}}\n".format(
                child.T, identifier, enumTmp
            )
            getMethods += "const {}& get{}Ref(size_t i) const {{ return __children.template getRef<{}>(i);}}\n".format(
                child.T, identifier, enumTmp
            )
            getMethods += "size_t get{}Size() const {{ return __children.template getSize<{}>();}}\n".format(
                identifier, enumTmp
            )
        else:
            print("Error generating: ", child)
        getMethods += "auto& get{}Raw() {{ return __children.template getRaw<{}>();}}\n".format(identifier, enumTmp)
        setMethods += "template <typename T> void set{}(T&& value) {{ __children.template set<{}>(std::forward<T>(value));}}\n".format(
            identifier, enumTmp
        )
        createMethods += "template <typename...Args> void create{}(Args&&...args) {{ __children.template create<{}>(std::forward<Args>(args)...);}}\n".format(
            identifier, enumTmp
        )
        hasMethods += "bool has{}() const {{ return __children.template has<{}>();}}".format(identifier, enumTmp)
        children.append("children_node<{}, {},{}{}>".format(kind, child.T, enumTmp, visit))
        enum.append(enumTmp)
    childrenMethods = (
        "children_t* operator->() { return &__children; }\n"
        "const children_t* operator->() const { return &__children; }\n"
        "children_t& operator*() { return __children; }\n"
        "const children_t& operator*() const { return __children; }\n"
        "children_t& children() { return __children; }\n"
        + "const children_t& children() const { return __children; }\n"
    )
    header = "enum {{{} endOffset}};".format(", ".join(enum) + ("," if len(enum) else ""))
    header += "using children_t = children_container<{}>;\n".format(", ".join(children))
    header += 'static_assert(children_t::size == endOffset, "Incongruent number of children.");\n'
    return (
        header,
        "children_t __children;",
        childrenMethods,
        getMethods,
        setMethods,
        pushMethods,
        createMethods,
        hasMethods,
    )


def generateVariable(var: Variable, node: Node):
    ID = var.identifier
    T = var.T
    variableName = "__" + ID
    variable = "{} {} = {{}};\n".format(T, variableName)
    getMethods = ""
    setMethods = ""
    pushMethods = ""
    isMethods = ""
    hasMethods = ""
    getMethods += "{}& get{}() {{\n return {};\n}}\n".format(T, ID.capitalize(), variableName)
    getMethods += "const {}& get{}() const {{\n return {};\n}}\n".format(T, ID.capitalize(), variableName)
    setMethods += "void set{}({}&& x) {{\n {} = std::forward<{}>(x);\n}}\n".format(ID.capitalize(), T, variableName, T)
    if isinstance(T, Vector):
        TT = T.underlying()
        pushMethods += "void push{}({}&& x) {{\n {}.push_back(std::forward<{}>(x));\n}}".format(
            ID.capitalize(), TT, variableName, TT
        )
    elif isinstance(T, UniquePtr):
        hasMethods += "bool has{}() const {{ return {};}}\n".format(ID.capitalize(), variableName)
    elif isinstance(T, EnumType):
        enum = node.types[T.typename()]
        for k in enum.enum:
            isMethods += "bool is{}() const {{\n return {} == {}; }}".format(k.name, variableName, k.name)
    return variable, getMethods, setMethods, pushMethods, isMethods, hasMethods


def generateMembers(node: Node):
    variables = ""
    getMethods = ""
    setMethods = ""
    pushMethods = ""
    isMethods = ""
    hasMethods = ""
    for var in node.members.values():
        v, gm, sm, pm, Is, hm = generateVariable(var, node)
        variables += v
        getMethods += gm
        setMethods += sm
        pushMethods += pm
        isMethods += Is
        hasMethods += hm
    return variables, getMethods, setMethods, pushMethods, isMethods, hasMethods


def generateSectionComment(section):
    return "/*{}*/\n".format("{:*^76s}".format("{: ^30s}".format(section)))


def generateSubTypes(node: Node):
    src = ""
    for enum in node.types.values():
        enumSrc = "enum {} {{ {} }};"
        b = ""
        for k in enum.enum:
            b += "{} = {},\n".format(k.name, k.value)
        src += enumSrc.format(enum.identifier, b) + "using {0:}_t = {0:};".format(enum.identifier)
    return src


def generateParent(node: Node):
    src = ""
    parents = node.parents.parents
    if len(parents) > 1:
        for i, p in enumerate(parents):
            src += "template <int I = 0, std::enable_if_t<I == {0:}, int> = 0> {1:}& getAsParent() {{ return  getAs<{1:}>(); }}\n".format(
                i, p
            )
            src += "template <int I = 0, std::enable_if_t<I == {0:}, int> = 0> const {1:}& getAsParent() const {{ return  getAs<{1:}>(); }}\n".format(
                i, p
            )
    else:
        src += "{0:}& getAsParent() {{ return  getAs<{0:}>(); }}\n".format(parents[0])
        src += "const {0:}& getAsParent() const {{ return  getAs<{0:}>(); }}\n".format(parents[0])
    return src


def generateClass(node: Node):
    classOf = node.classOf + "Class"
    src = ""
    public = "public:\n" + generateSectionComment("Class Header") + generateHeader(node, classOf)
    protected = ""
    accessors = ""
    methods = ""
    variables = ""
    h, v, cm, gm, sm, pm, Cm, hm = generateChildren(node)
    public += h
    public += generateSectionComment("Class Subtypes") + generateSubTypes(node)
    public += generateSectionComment("Constructors & Operators") + generateConstructors(node)
    public += generateSectionComment("Parent operations") + generateParent(node)
    public += generateSectionComment("Children accessors") + cm
    mv, mgm, msm, mpm, mim, mhm = generateMembers(node)
    variables = "protected:\n" + v + mv
    methods += hm + mhm + mim + Cm
    accessors += gm + mgm + sm + msm + pm + mpm
    public += generateSectionComment("Accessors") + accessors + generateSectionComment("Methods") + methods
    protected += variables
    src += public + protected
    return "class {} {{\n{}}};".format(node.typename() + generateParents(node), src)


def generateNodesHeader(nodesByClass: dict):
    ID = "NodeClass"
    fwd = ""
    enum = ""
    to_string = ""
    is_methods = ""
    for k, nodes in nodesByClass.items():
        n = len(nodes.values())
        for i, node in enumerate(nodes.values()):
            fwd += "class {};\n".format(node.typename())
            if i == 0:
                enum += "First{},\n".format(k, node.typename())
            enum += node.typename() + ",\n"
            if i == (n - 1):
                enum += "Last{},\n".format(k, node.typename())
            to_string += 'case {0:}::{1:}:\n return "{1:}";'.format(ID, node.typename())
        is_methods += "inline constexpr bool is{0:}({1:} kind) {{ return (NodeClass::First{0:} < kind) && (kind < NodeClass::Last{0:});}}".format(
            k, ID
        )
    src = "enum class {} {{\nASTNode,\nASTNodeList,\n{}}};".format(ID, enum)
    src += is_methods
    src += 'inline std::string to_string({0:} kind) {{switch(kind){{{1:}default: return "Unknown {0:}";}}}}'.format(
        ID, to_string
    )
    src += fwd
    return "#ifndef __AST_{0:}__\n#define __AST_{0:}__\n#include <string>\nnamespace _astnp_ {{\n{1:}}}\n#endif".format(
        "NODES_HEADER", src
    )


def generateRecursive(nodes: dict):
    visit = ""
    walkup = ""
    traverse = ""
    traverse_cases = ""
    for node in nodes.values():
        visit += "bool visit{0:}({0:}* node) {{ return true; }}\n".format(node.typename())
        walkups = ""
        for p in node.parents.parents:
            if p.typename() != "DeclContext":
                walkups += "WALKUP_STMT({1:}, {0:})\n".format(node.typename(), p)
        walkup += "bool walkUpTo{0:}({0:}* node) {{ {1:} }}\n".format(node.typename(), walkups)
        traverse += "bool traverse{0:}({0:}* node, stack_t *stack = nullptr) {{ TRAVERSE_STMT({0:}) }}\n".format(
            node.typename()
        )
        traverse_cases += "case NodeClass::{0:}: return derived.traverse{0:}(node->getAsPtr<{0:}>(), stack);\n".format(
            node.typename()
        )
    return {"VISIT": visit, "WALK_UP": walkup, "TRAVERSE_CASES": traverse_cases, "TRAVERSE": traverse}


from Utility.format import format


def generateAstNodes(grammar: LangDB, outdir: str, inputDir="Templates"):
    with open(pathlib.Path(inputDir, "recursive_ast_visitor.hh.j2"), "r") as file:
        j2 = Template(file.read())
        with open(pathlib.Path(outdir, "recursive_ast_visitor.hh"), "w") as file:
            print(j2.render(generateRecursive(grammar.nodes)), file=file)
        format(pathlib.Path(outdir, "recursive_ast_visitor.hh"))
    header = generateNodesHeader(grammar.nodesByClass)
    with open(pathlib.Path(outdir, "nodes.hh"), "w") as file:
        print(header, file=file)

    format(pathlib.Path(outdir, "nodes.hh"))
    for classOf, nodes in grammar.nodesByClass.items():
        structs = ""
        for node in nodes.values():
            structs += generateClass(node) + "\n\n"
        src = "#ifndef __AST_{0:}__\n#define __AST_{0:}__\n#include <ast_node.hh>\n\nnamespace _astnp_ {{\n{1:}}}\n#endif".format(
            classOf.upper(), structs
        )
        with open(pathlib.Path(outdir, classOf.lower() + ".hh"), "w") as file:
            print(src, file=file)

        format(pathlib.Path(outdir, classOf.lower() + ".hh"))
