#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Lexer.token import Character, Definition, Keyword, Literal, Rule

lexerDefinitions = {}
lexerTokens = {}


class LexerTokens:
    __dict__ = lexerTokens.keys

    def __getattr__(self, attr):
        return lexerTokens.get(attr)


LToks = LexerTokens()


def addToken(kind, identifier, *rules):
    lexerTokens[identifier] = kind(identifier, *rules if len(rules) > 0 else None)


def addDefinition(identifier, *rules):
    lexerDefinitions[identifier] = Definition(identifier, *rules)


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
addToken(Keyword, "Task")
addToken(Keyword, "Policy")
addToken(Keyword, "Parallel")
addToken(Keyword, "Sync")
addToken(Keyword, "Async")
addToken(Keyword, "Wait")
addToken(Keyword, "Atomic")
# ******************************************************************************
#   Language constructs
# ******************************************************************************
addToken(Keyword, "Function")
addToken(Keyword, "Struct")
addToken(Keyword, "Enum")
addToken(Keyword, "Loop")
addToken(Keyword, "For")
addToken(Keyword, "While")
addToken(Keyword, "Continue")
addToken(Keyword, "If")
addToken(Keyword, "Else")
addToken(Keyword, "Switch")
addToken(Keyword, "Case")
addToken(Keyword, "Break")
addToken(Keyword, "Default")
addToken(Keyword, "Import")
addToken(Keyword, "Const")
addToken(Keyword, "Inline")
addToken(Keyword, "Using")
addToken(Keyword, "Return")
addToken(Keyword, "Arch")
addToken(Keyword, "Ccall")
addToken(Keyword, "Constexpr")
addToken(Keyword, "Alignment")
addToken(Keyword, "Auto")
# ******************************************************************************
#   Memory Kinds
# ******************************************************************************
addToken(Keyword, "Global")
addToken(Keyword, "Shared")
addToken(Keyword, "Local")
addToken(Keyword, "Constant")
# ******************************************************************************
#   Fundamental types
# ******************************************************************************
addToken(Keyword, "Void")
addToken(Keyword, "Bool")
addToken(Keyword, "Int")
addToken(Keyword, "Float")
addToken(Keyword, "Complex")
addToken(Keyword, "Array")
addToken(Keyword, "Vector")
addToken(Keyword, "Matrix")
addToken(Keyword, "Tensor")
addToken(Keyword, "String")
# ******************************************************************************
#
#   Literals & Identifiers
#
# ******************************************************************************
addToken(Keyword, "True")
addToken(Keyword, "False")
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
