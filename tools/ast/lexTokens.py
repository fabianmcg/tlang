#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from token import Character, Definition, Keyword, Literal, Rule

lexerTokens = {}
lexerDefinitions = {}


def addToken(kind, identifier, *rules):
    lexerTokens[identifier] = kind(identifier, *rules)


def addDefinition(identifier, *rules):
    lexerDefinitions[identifier] = Definition(identifier, *rules)

class LexToks:
    __dict__ = lexerTokens.keys
    def __getattr__(self, attr):
        return lexerTokens.get(attr)

LTok = LexToks()

# ******************************************************************************
#
#   Rules
#
# ******************************************************************************
addDefinition("D", r"[0-9]")
addDefinition("NZ", r"[1-9]")
addDefinition("L", r"[a-zA-Z_]")
addDefinition("A", r"[a-zA-Z_0-9]")
addDefinition("E", r"([Ee][+-]?{D}+)")
addDefinition("WS", r"[ \t\v\f]")
# ******************************************************************************
#
#   Keywords
#
# ******************************************************************************
# ******************************************************************************
#   Parallel constructs
# ******************************************************************************
addToken(Keyword, "Task", "task")
addToken(Keyword, "Policy", "policy")
addToken(Keyword, "Arch", "arch")
addToken(Keyword, "Parallel", "parallel")
addToken(Keyword, "Sync", "sync")
addToken(Keyword, "Async", "async")
addToken(Keyword, "Wait", "wait")
addToken(Keyword, "Atomic", "atomic")
# ******************************************************************************
#   Language constructs
# ******************************************************************************
addToken(Keyword, "Function", "function")
addToken(Keyword, "Struct", "struct")
addToken(Keyword, "Enum", "enum")
addToken(Keyword, "Loop", "loop")
addToken(Keyword, "For", "for")
addToken(Keyword, "While", "while")
addToken(Keyword, "Continue", "continue")
addToken(Keyword, "If", "if")
addToken(Keyword, "Else", "else")
addToken(Keyword, "Switch", "switch")
addToken(Keyword, "Case", "case")
addToken(Keyword, "Break", "break")
addToken(Keyword, "Default", "default")
addToken(Keyword, "Import", "import")
addToken(Keyword, "Const", "const")
addToken(Keyword, "Inline", "inline")
addToken(Keyword, "Using", "using")
addToken(Keyword, "Return", "return")
addToken(Keyword, "Ccall", "ccall")
addToken(Keyword, "Constexpr", "constexpr")
addToken(Keyword, "Alignment", "alignment")
addToken(Keyword, "Auto", "auto")
# ******************************************************************************
#   Memory Kinds
# ******************************************************************************
addToken(Keyword, "Global", "global")
addToken(Keyword, "Shared", "shared")
addToken(Keyword, "Local", "local")
addToken(Keyword, "Constant", "constant")
# ******************************************************************************
#   Fundamental types
# ******************************************************************************
addToken(Keyword, "Void", "void")
addToken(Keyword, "Bool", "bool")
addToken(Keyword, "Int", "int")
addToken(Keyword, "Float", "float")
addToken(Keyword, "Complex", "complex")
addToken(Keyword, "Array", "array")
addToken(Keyword, "Vector", "vector")
addToken(Keyword, "Matrix", "matrix")
addToken(Keyword, "Tensor", "tensor")
addToken(Keyword, "String", "string")
# ******************************************************************************
#
#   Literals & Identifiers
#
# ******************************************************************************
addToken(Keyword, "True", "true")
addToken(Keyword, "False", "false")
addToken(Rule, "IntLiteral", r"{NZ}{D}*")
addToken(Rule, "FloatLiteral", r"{D}+{E}", r'{D}*"."{D}+{E}?')
addToken(Rule, "Identifier" r"{L}{A}*")
# ******************************************************************************
#
#   Operands
#
# ******************************************************************************
addToken(Literal, "And", "&&")
addToken(Literal, "Or", "||")
addToken(Literal, "Equal", "==")
addToken(Literal, "NEQ", "!=")
addToken(Literal, "LEQ", "<=")
addToken(Literal, "GEQ", ">=")
addToken(Literal, "LShift", "<<")
addToken(Literal, "RShift", ">>")
addToken(Literal, "Pow", "^^")
addToken(Literal, "Namespace", "::")
# ******************************************************************************
#   Characters
# ******************************************************************************
addToken(Character, "Not", "!")
addToken(Character, "Band", "&")
addToken(Character, "Bor", "|")
addToken(Character, "Less", "<")
addToken(Character, "Greater", ">")
addToken(Character, "Assign", "=")
addToken(Character, "Plus", "+")
addToken(Character, "Minus", "-")
addToken(Character, "Multiply", "*")
addToken(Character, "Divide", "/")
addToken(Character, "At", "@")
addToken(Character, "Dot", ".")
addToken(Character, "Comma", ",")
addToken(Character, "Colon", ":")
addToken(Character, "Semicolon", ";")
addToken(Character, "Ternary", "?")
addToken(Character, "LBrace", "{")
addToken(Character, "RBrace", "}")
addToken(Character, "LParen", "(")
addToken(Character, "RParen", ")")
addToken(Character, "LBrack", "[")
addToken(Character, "RBrack", "]")
