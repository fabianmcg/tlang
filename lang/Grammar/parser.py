#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

import pyparsing as pp


class AbstractNode:
    def __init__(self, identifier, instruction=None) -> None:
        self.identifier = identifier
        self.instruction = instruction

    def hasInstruction(self):
        return self.instruction != None

    def identifierStr(self):
        return self.identifier

    def __str__(self) -> str:
        instruction = " {}".format(self.instruction) if self.hasInstruction() else ""
        return "{}{}".format(self.identifierStr(), instruction)

    __repr__ = __str__


class Terminal(AbstractNode):
    def __init__(self, identifier, instruction=None) -> None:
        super().__init__(identifier, instruction=instruction)

    def identifierStr(self) -> str:
        return "T::" + self.identifier


class NonTerminal(AbstractNode):
    def __init__(self, identifier, instruction=None) -> None:
        super().__init__(identifier, instruction=instruction)


class Instruction:
    def __init__(self, instruction) -> None:
        if isinstance(instruction, str):
            instruction = instruction.strip()
        self.instruction = instruction

    def getAsStr(self):
        return "{{{}}}".format(self.instruction)

    def __str__(self) -> str:
        return self.getAsStr()

    __repr__ = __str__


class NodeInstruction(Instruction):
    def __init__(self, instruction) -> None:
        super().__init__(instruction)

    def getAsStr(self):
        return "@{{ {} }}@".format(self.instruction)


class RuleInstruction(Instruction):
    def __init__(self, instruction) -> None:
        super().__init__(instruction)

    def getAsStr(self):
        return ":{{ {} }}:".format(self.instruction)


class Rule:
    def __init__(self, rule, instruction) -> None:
        self.rule = rule
        self.instruction = instruction

    def hasInstruction(self):
        return self.instruction != None

    def __str__(self) -> str:
        instruction = " {}".format(self.instruction) if self.hasInstruction() else ""
        return "{}{}".format(" ".join(map(str, self.rule)), instruction)

    __repr__ = __str__


class ProductionAttributes:
    def __init__(self, returnType=None, isNode=False, instruction=None) -> None:
        if isinstance(returnType, str):
            returnType = returnType.strip()
        self.returnType = returnType
        self.isNode = isNode
        self.instruction = instruction[0] if len(instruction) else None

    def hasInstruction(self):
        return self.instruction != None

    def isDefault(self):
        return (self.returnType == None) and (self.isNode == False) and (self.instruction == None)

    def __str__(self) -> str:
        text = " @< {} >@".format(self.returnType) if self.returnType else ""
        text += " node" if self.isNode else ""
        text += " {}".format(self.instruction) if self.hasInstruction() else ""
        return text

    __repr__ = __str__


class Production:
    def __init__(self, identifier, attributes, rules) -> None:
        self.identifier = identifier
        self.attributes = attributes
        self.rules = rules

    def __str__(self) -> str:
        n = len(self.rules)
        pad = "" if n == 1 else " " * 8
        rules = (pad * 2) + "{}{}".format(str(self.rules[0]), "\n" if n > 1 else "")
        for rule in self.rules[1:]:
            rules += (" " * 8) + "|" + (" " * 7) + "{}\n".format(str(rule))
        if n > 1:
            return "{:}{:}:=\n{}".format(self.identifier, self.attributes, rules) + (" " * 8) + ";\n"
        return "{:}{:}:= {}".format(self.identifier, self.attributes, rules) + pad + ";\n"

    __repr__ = __str__


def suppressChars(chars):
    if len(chars) > 1:
        return map(lambda x: pp.Char(x).suppress(), chars)
    return pp.Char(chars).suppress()


def suppressLiterals(*literals):
    if len(literals) > 1:
        return map(lambda x: pp.Literal(x).suppress(), literals)
    return pp.Literal(literals[0]).suppress()


class Parser:
    identifier = pp.Word(pp.alphas + "_", pp.alphanums + "_")

    @staticmethod
    def parseElement(parseElement, action=None):
        if action == None:
            return parseElement
        x = parseElement.copy()
        x.setParseAction(action)
        return x

    @staticmethod
    def parseTerminal(x):
        return Terminal(str(x[0]))

    @staticmethod
    def parseNonTerminal(x):
        return NonTerminal(str(x[0]))

    @staticmethod
    def parseInstruction(instructionClass):
        def parse(x):
            return instructionClass(x[0])

        return parse

    @staticmethod
    def parseRuleNode(x):
        return type(x[0])(x[0].identifier, *(x[1:]))

    @staticmethod
    def parseRule(x):
        return Rule(x[0], x[1] if len(x) > 1 else None)

    @staticmethod
    def parseProductionAttributes(x):
        return ProductionAttributes(**{k: v for k, v in x})

    @staticmethod
    def parseProduction(x):
        return Production(x[0], ProductionAttributes(), x[1]) if len(x) == 2 else Production(x[0], x[1], x[2])

    @staticmethod
    def createInstruction(begin, end):
        def makeLiteral(literal):
            return suppressChars(literal) if len(literal) == 1 else suppressLiterals(literal)

        return Parser.parseElement(
            makeLiteral(begin) + ... + makeLiteral(end), Parser.parseInstruction(NodeInstruction)
        )

    @staticmethod
    def createRuleNode():
        token = suppressLiterals("T::")
        terminal = Parser.parseElement(token + Parser.identifier, Parser.parseTerminal)
        nonterminal = Parser.parseElement(Parser.identifier, Parser.parseNonTerminal)
        instruction = Parser.createInstruction("@{", "}@")
        return Parser.parseElement((terminal | nonterminal) + pp.Optional(instruction), Parser.parseRuleNode)

    @staticmethod
    def createRule():
        ruleNode = Parser.createRuleNode()
        instruction = Parser.parseElement(Parser.createInstruction(":{", "}:"), Parser.parseInstruction(RuleInstruction))
        return Parser.parseElement(pp.Group(pp.ZeroOrMore(ruleNode)) + pp.Optional(instruction), Parser.parseRule)

    @staticmethod
    def createProductionAttributes():
        lat, rat = suppressLiterals("@<", ">@")
        instruction = Parser.parseElement(Parser.createInstruction(":{", "}:"), Parser.parseInstruction(RuleInstruction))
        returnType = pp.Optional(Parser.parseElement(lat + ... + rat, lambda x: ("returnType", x[0])))
        inline = pp.Optional(Parser.parseElement(pp.Keyword("node"), lambda x: ("isNode", True)))
        instruction = Parser.parseElement(pp.Optional(instruction), lambda x: ("instruction", x))
        return Parser.parseElement(returnType + inline + instruction, Parser.parseProductionAttributes)

    @staticmethod
    def createProduction():
        pipe = suppressChars("|")
        equal = suppressLiterals(":=")
        rule = Parser.createRule()
        semicolon = suppressChars(";")
        attributes = Parser.createProductionAttributes()
        return Parser.parseElement(
            Parser.identifier + attributes + equal + pp.Group(rule + pp.ZeroOrMore(pipe + rule) + semicolon),
            Parser.parseProduction,
        )

    @staticmethod
    def createProductionList():
        return pp.OneOrMore(Parser.createProduction()).ignore(pp.cStyleComment)

    @staticmethod
    def parse(filename):
        pp._enable_all_warnings()
        result = Parser.createProductionList().parseFile(filename, parseAll=True)
        print("\n".join(map(str, list(result))))


def parse(filename):
    parser = Parser.parse(filename)
