#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Grammar.parseElements import AbstractNode, EmptyString, NodeInstruction, NonTerminal, Production, Terminal, Rule
from Lexer.lexer import Lexer
from Utility.dotDict import DotDict
from Utility.util import formatStr, getShortRepr, jinjaTemplate, format
import graph_tool as gt


class Grammar:
    def __init__(self, lexer: Lexer):
        self.tokens = {tok.parseRepr(): tok for tok in lexer.tokens.values()}
        self.terminals = {}
        self.nonTerminals = {}
        self.productions = {}

    def __str__(self) -> str:
        return "{}".format("\n".join(list(map(str, self.productions))))

    def __repr__(self) -> str:
        return str(self)

    def setProductions(self, productions: list):
        self.productions = {production.identifier: production for production in productions}
        self.consolidate()

    def shortRepr(self) -> str:
        return "{}".format("\n".join(list(map(getShortRepr, self.productions.values()))))

    def consolidate(self):
        for production in self.productions.values():
            pnt = production.asNonTerminal()
            if production not in self.nonTerminals:
                self.nonTerminals[pnt] = pnt
            for rule in production:
                for node in rule:
                    if isinstance(node, Terminal):
                        self.terminals[node] = node
                    elif isinstance(node, NonTerminal):
                        self.nonTerminals[node] = node
                    else:
                        raise (Exception("Invalid node"))

    def cxxNode(self, index, node: AbstractNode):
        varName = "_{}".format(index)
        if isinstance(node, EmptyString):
            return ""
        elif isinstance(node, Terminal):
            return "auto {0:} = tok; if ({0:}.isNot({1:})) return return_t::fail(); consumeToken();".format(
                varName, node.identifier.cxx()
            )
        elif isinstance(node, NonTerminal):
            return "auto {0:} = Parse{1:}(); if (!{0:}) return return_t::fail();".format(varName, node.identifier)
        return ""

    def cxxRule(self, rule: Rule):
        if rule.isEmpty():
            return "{{ return_t _r = return_t::empty();{}return _r; }}".format(rule.instruction.cxx())
        body = "\n".join(map(lambda x: self.cxxNode(*x), enumerate(rule.rule))) + rule.instruction.cxx()
        return "{{auto ctx = getContext(); auto parse = [&]() -> return_t{{ return_t _r{{}};{} return _r;}}; auto _result = parse(); if (_result) return _result; restoreContext(ctx); }}".format(
            body
        )

    def cxxProduction(self, production: Production):
        if production.identifier == "__top__":
            return "auto ParseTop() {{ return Parse{}(); }}".format(str(production[0][0]))
        name = "Parse{}".format(production.identifier)
        returnType = "parse_return_t<{}>"
        if production.attributes.hasReturnType():
            returnType = returnType.format(production.attributes.returnType)
        elif production.attributes.isNode:
            returnType = returnType.format("{}".format(production.identifier))
        else:
            print("Error the {} production has no return type.".format(production.identifier))
        body = "using return_t = {};{}".format(returnType, production.attributes.instruction)
        body += "\n".join(map(lambda x: self.cxxRule(x), production.rules))
        return "{} {}() {{\n{}\n return return_t::fail();}}".format(returnType, name, body)

    def cxx(self):
        return "\n".join(map(lambda x: self.cxxProduction(x), self.productions.values()))

    def generateCxx(self, outputPath: str, templatePath: str):
        jinjaTemplate(outputFile=outputPath, inputFile=templatePath, templateDict={"PARSE_METHODS": self.cxx()})
        format(outputPath)


class GrammarGraph:
    def __init__(self, grammar: Grammar, entryProduction: str = "__top__"):
        self.entryProduction = entryProduction
        self.grammar = grammar
        self.graph = gt.Graph()
        self.terminals = {}
        self.nonTerminals = {}
        self.nodeToVertex = {}
        self.vertexToNode = {}
        self.consolidate()
        self.createGraph()

    def addVertexProperty(self, name, type, values=None, value=None):
        property = self.graph.new_vertex_property(type, vals=values, val=value)
        self.graph.vertex_properties[name] = property
        return property

    def addEdgeProperty(self, name, type, values=None, value=None):
        property = self.graph.new_edge_property(type, vals=values, val=value)
        self.graph.edge_properties[name] = property
        return property

    def orderedNodes(self):
        return sorted(self.nodeToVertex.keys(), key=lambda x: self.nodeToVertex[x])

    def saveGraph(self, filename):
        nodes = self.orderedNodes()
        G = self.graph
        self.addVertexProperty("label", "string", values=[node.identifier for node in nodes])
        self.addVertexProperty(
            "shape",
            "string",
            values=[("doublecircle" if isinstance(node, NonTerminal) else "circle") for node in nodes],
        )
        self.addEdgeProperty("style", "string", ["solid" if G.ep.type[e] == 0 else "dotted" for e in G.edges()])
        self.addEdgeProperty("color", "string", ["green" if G.ep.type[e] == 0 else "red" for e in G.edges()])
        self.graph.save(filename + ".graphml", "graphml")
        self.graph.save(filename + ".dot", "dot")

    def consolidate(self):
        productions = self.grammar.productions
        productionStack = [self.entryProduction]
        visited = set([])
        vid = [0]

        def addId(node):
            if node not in self.nodeToVertex:
                self.nodeToVertex[node] = vid[0]
                self.vertexToNode[vid[0]] = node
                vid[0] += 1

        while len(productionStack):
            pid = productionStack.pop()
            if pid not in productions or pid in visited:
                continue
            visited.add(pid)
            production = productions[pid]
            addId(production.asNonTerminal())
            for rule in production:
                for node in rule:
                    if isinstance(node, Terminal):
                        self.terminals[node] = node
                    elif isinstance(node, NonTerminal):
                        self.nonTerminals[node] = node
                        if node.identifier in productions:
                            productionStack.append(node.identifier)
                    else:
                        raise (Exception("Invalid node"))
                    addId(node)

    def createGraph(self):
        G = self.graph
        G.add_vertex(len(self.nodeToVertex))
        edgeType = self.addEdgeProperty("type", "int")
        productions = self.grammar.productions
        productionStack = [self.entryProduction]
        visited = set([])
        while len(productionStack):
            pid = productionStack.pop()
            if pid not in productions or pid in visited:
                continue
            visited.add(pid)
            production = productions[pid]
            pv = G.vertex(self.nodeToVertex[production.asNonTerminal()])
            for rule in production:
                pnv = pv
                for i, node in enumerate(rule):
                    nv = G.vertex(self.nodeToVertex[node])
                    e = G.add_edge(pnv, nv)
                    edgeType[e] = 0 if i == 0 else 1
                    pnv = nv
                    if node.identifier in productions:
                        productionStack.append(node.identifier)
