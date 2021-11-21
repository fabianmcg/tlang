#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

import pathlib
from jinja2 import Template
from Grammar.db import GrammarDB
from Grammar.instruction import *
from Grammar.rule import *
from Utility.type import Optional as OptionalType, TemplateType


def generateIf(Condition, Then, Else=None, Comment=None, SingleThen=False, SingleElse=False):
    c = "// " + Comment if Comment != None else ""
    tb = "{}\n {}" if SingleThen else "{{{}\n {}}}\n"
    eb = "{}" if SingleElse else "{{{}}}\n"
    return (
        (("if ({})" + tb).format(Condition, c, Then))
        if Else == None
        else (("if ({})" + tb + "else\n" + eb).format(Condition, c, Then, Else))
    )


class RuleGenerator:
    def __init__(self, template) -> None:
        self.template = template
        self.counters = {}
        self.variables = {}
        self.nodesStack = []
        self.nullRet = "return optret_t {};"
        self.returnType = None
        self.labels = ["RET"]
        self.topRule = []
        self.insideLoop = False
        self.produceMetadata = True

    def getId(self, label):
        if label not in self.counters:
            self.counters[label] = 0
        c = self.counters[label]
        self.counters[label] += 1
        return c

    def handleInstruction(self, instruction: Instruction):
        if isinstance(instruction, VariableRef):
            return "result." + instruction.instruction
        elif isinstance(instruction, AssignInstruction):
            lhs = self.handleInstruction(instruction.lhs)
            if isinstance(instruction.rhs, NonTerminal):
                rhs = self.variables[str(instruction.rhs.identifier)]
            else:
                rhs = "tok.value()"
            return "{} = {};".format(lhs, rhs)
        elif isinstance(instruction, AppendInstruction):
            lhs = self.handleInstruction(instruction.lhs)
            if isinstance(instruction.rhs, NonTerminal):
                rhs = "*" + self.variables[str(instruction.rhs.identifier)]
            else:
                rhs = "tok.value()"
            return "{}.push_back({});".format(lhs, rhs)
        return ""

    def handleMetadata(self, metadata: Metadata):
        if not self.produceMetadata:
            return ""
        if metadata != None and isinstance(metadata, Instruction):
            return self.handleInstruction(metadata)
        return ""

    def handleEmpty(self, rule: EmptyProduction):
        src = ""
        return src

    def handleTerminal(self, rule: Terminal):
        src = "tok = consumeTok();\n"
        I = rule.identifier.identifier
        if self.insideLoop:
            src += generateIf("tok != _lnp_::tok_k::" + I, "break;", Comment="Token: " + I, SingleThen=True)
        elif len(self.topRule):
            lastRule = self.topRule[-1]
            stmt = "goto {};".format(self.labels[-1])
            if isinstance(lastRule, And):
                src += generateIf("tok != _lnp_::tok_k::" + I, stmt, Comment="Token: " + I, SingleThen=True)
            else:
                src += generateIf("tok == _lnp_::tok_k::" + I, stmt, Comment="Token: " + I)
        else:
            src += generateIf("tok != _lnp_::tok_k::" + I, "goto RET;", Comment="Token: " + I, SingleThen=True)
        src += self.handleMetadata(rule.metadata)
        return src

    def handleNonTerminal(self, rule: NonTerminal):
        self.variables[rule.identifier] = var = "_{0:}{1:}".format(rule.identifier, self.getId(rule.identifier))
        src = "auto {1:} = handle{0:}();".format(rule.identifier, var)
        src += generateIf("!" + var, self.nullRet, Comment="Nonterminal: " + rule.identifier)
        src += self.handleMetadata(rule.metadata)
        return src

    def handleZeroOrMore(self, rule: ZeroOrMore):
        self.insideLoop = True
        var = "_count{}".format(self.getId("_count"))
        src = "size_t {0:} = 0;while (true) {{ \n{1:} ++{0:}; }}".format(var, self.handleRule(rule.node))
        self.insideLoop = False
        return src

    def handleOneOrMore(self, rule: OneOrMore):
        self.insideLoop = True
        var = "_count{}".format(self.getId("_count"))
        src = "size_t {0:} = 0;while (true) {{ \n{1:} ++{0:}; }}".format(var, self.handleRule(rule.node))
        self.insideLoop = False
        return src

    def handleOptional(self, rule: Optional):
        src = ""
        return src

    def handleAnd(self, rule: And):
        src = ""
        for term in rule.nodes:
            src += self.handleRule(term)
        return src

    def handleOr(self, rule: Or):
        src = ""
        for term in rule.nodes:
            src += self.handleRule(term)
        return src

    def preRule(self, rule: RuleNode):
        self.nodesStack.append(rule)
        src = ""
        # src = "/** BEGIN: {} **/\n".format(rule.__class__.__name__)
        if isinstance(rule, And) or isinstance(rule, Or):
            self.topRule.append(rule)
            name = rule.__class__.__name__
            self.labels.append("{}{}".format(name, self.getId(name)))
        return src

    def postRule(self, rule: RuleNode):
        self.nodesStack.pop()
        src = ""
        # src = "/** END:   {} **/\n".format(rule.__class__.__name__)
        if isinstance(rule, And) or isinstance(rule, Or):
            label = self.labels.pop()
            self.topRule.pop()
            src += "{}:\n".format(label)
        return src

    def handleRule(self, rule: RuleNode):
        src = ""
        src += self.preRule(rule)
        if isinstance(rule, EmptyProduction):
            src += self.handleEmpty(rule)
        elif isinstance(rule, Terminal):
            src += self.handleTerminal(rule)
        elif isinstance(rule, NonTerminal):
            src += self.handleNonTerminal(rule)
        elif isinstance(rule, ZeroOrMore):
            src += self.handleZeroOrMore(rule)
        elif isinstance(rule, OneOrMore):
            src += self.handleOneOrMore(rule)
        elif isinstance(rule, Optional):
            src += self.handleOptional(rule)
        elif isinstance(rule, And):
            src += self.handleAnd(rule)
        elif isinstance(rule, Or):
            src += self.handleOr(rule)
        src += self.postRule(rule)
        return src

    def topHandle(self, rule):
        src = "_lnp_::token_t tok;\nresult_t result;\n{}RET:\n{}"
        return src.format(self.handleRule(rule), self.nullRet)

    def handleProductionRule(self, rule: Rule):
        self.returnType = rule.returnType
        opt_t = str(OptionalType(rule.returnType))
        ret_t = str(rule.returnType)
        res_t = str(rule.returnType.underlying()) if isinstance(rule.returnType, TemplateType) else ret_t
        rules = []
        for r in rule.rules:
            rules.append({"str": str(r), "body": self.topHandle(r)})
        return self.template.render(
            {"id": rule.identifier, "opt_t": opt_t, "ret_t": ret_t, "res_t": res_t, "rules": rules}
        )


def generateParser(grammar: GrammarDB, outdir: str, inputDir="Templates"):
    source = ""
    with open(pathlib.Path(inputDir, "parserMethod.j2"), "r") as file:
        j2 = Template(file.read())
    for r in grammar.rules.values():
        generator = RuleGenerator(j2)
        source += generator.handleProductionRule(r)
    with open(pathlib.Path(outdir, "parser.cc"), "w") as file:
        print(source, file=file)
    from Utility.format import format

    format(pathlib.Path(outdir, "parser.cc"))
