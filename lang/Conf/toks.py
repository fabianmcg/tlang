#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""
# ******************************************************************************
#
#   Header
#
# ******************************************************************************
from Lexer.token import Punctuation, Keyword, Rule, Operator
from Lexer.lexer import Lexer


def makeLexer():
    db = Lexer()
    # ******************************************************************************
    #
    #   Rules
    #
    # ******************************************************************************
    db.addDefinition("D", r"[0-9]")
    db.addDefinition("NZ", r"[1-9]")
    db.addDefinition("L", r"[a-zA-Z_]")
    db.addDefinition("A", r"[a-zA-Z_0-9]")
    db.addDefinition("E", r"([Ee][+-]?{D}+)")
    db.addDefinition("ES", r"(\\(['\"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))")
    db.addDefinition("WS", r"[ \t\v\f]")
    # ******************************************************************************
    #
    #   Keywords
    #
    # ******************************************************************************
    # ******************************************************************************
    #   Parallel constructs
    # ******************************************************************************
    db.addToken(Keyword, "Task")
    db.addToken(Keyword, "Policy")
    db.addToken(Keyword, "Parallel")
    db.addToken(Keyword, "Context")
    db.addToken(Keyword, "Sync")
    db.addToken(Keyword, "Async")
    db.addToken(Keyword, "Wait")
    db.addToken(Keyword, "Atomic")
    db.addToken(Keyword, "__allocator__")
    db.addToken(Keyword, "__deleter__")
    db.addToken(Keyword, "__parallel__")
    db.addToken(Keyword, "__executor__")
    # ******************************************************************************
    #   Language constructs
    # ******************************************************************************
    db.addToken(Keyword, "Typeof")
    db.addToken(Keyword, "Function")
    db.addToken(Keyword, "Struct")
    db.addToken(Keyword, "Let")
    db.addToken(Keyword, "Enum")
    db.addToken(Keyword, "Loop")
    db.addToken(Keyword, "For")
    db.addToken(Keyword, "While")
    db.addToken(Keyword, "Continue")
    db.addToken(Keyword, "If")
    db.addToken(Keyword, "Else")
    db.addToken(Keyword, "Switch")
    db.addToken(Keyword, "Case")
    db.addToken(Keyword, "Break")
    db.addToken(Keyword, "Default")
    db.addToken(Keyword, "Import")
    db.addToken(Keyword, "Const")
    db.addToken(Keyword, "Inline")
    db.addToken(Keyword, "Using")
    db.addToken(Keyword, "Return")
    db.addToken(Keyword, "Arch")
    db.addToken(Keyword, "Ccall")
    db.addToken(Keyword, "Constexpr")
    db.addToken(Keyword, "Alignment")
    db.addToken(Keyword, "Auto")
    db.addToken(Keyword, "Namespace")
    db.addToken(Keyword, "This")
    # ******************************************************************************
    #   Memory Kinds
    # ******************************************************************************
    db.addToken(Keyword, "Global")
    db.addToken(Keyword, "Shared")
    db.addToken(Keyword, "Local")
    db.addToken(Keyword, "Constant")
    # ******************************************************************************
    #   Fundamental types
    # ******************************************************************************
    db.addToken(Keyword, "Void")
    db.addToken(Keyword, "Bool")
    db.addToken(Keyword, "Int")
    db.addToken(Keyword, "Uint")
    db.addToken(Keyword, "Float")
    db.addToken(Keyword, "Complex")
    db.addToken(Keyword, "Array")
    db.addToken(Keyword, "Vector")
    db.addToken(Keyword, "Matrix")
    db.addToken(Keyword, "Tensor")
    db.addToken(Keyword, "String")
    # ******************************************************************************
    #
    #   Literals & Identifiers
    #
    # ******************************************************************************
    db.addToken(Keyword, "True")
    db.addToken(Keyword, "False")
    db.addToken(Rule, "IntLiteral", r"{NZ}{D}*", r'"0"')
    db.addToken(Rule, "FloatLiteral", r"{D}+{E}", r'{D}*"."{D}+{E}?')
    db.addToken(Rule, "StringLiteral", r'\"([^"\\\n]|{ES})*\"')
    db.addToken(Rule, "Identifier", r"{L}{A}*")
    # ******************************************************************************
    #
    #   Punctuation
    #
    # ******************************************************************************
    db.addToken(Operator, "And", "&&")
    db.addToken(Operator, "Or", "||")
    db.addToken(Operator, "Equal", "==")
    db.addToken(Operator, "NEQ", "!=")
    db.addToken(Operator, "LEQ", "<=")
    db.addToken(Operator, "GEQ", ">=")
    db.addToken(Operator, "LShift", "<<")
    db.addToken(Operator, "RShift", ">>")
    db.addToken(Operator, "Pow", "^^")
    db.addToken(Operator, "Namespace", "::")
    db.addToken(Operator, "Increment", "++")
    db.addToken(Operator, "Decrement", "--")
    # ******************************************************************************
    #   Characters
    # ******************************************************************************
    db.addToken(Operator, "Not", "!")
    db.addToken(Operator, "Band", "&")
    db.addToken(Operator, "Bor", "|")
    db.addToken(Operator, "Less", "<")
    db.addToken(Operator, "Greater", ">")
    db.addToken(Operator, "Assign", "=")
    db.addToken(Operator, "Plus", "+")
    db.addToken(Operator, "Minus", "-")
    db.addToken(Operator, "Multiply", "*")
    db.addToken(Operator, "Divide", "/")
    db.addToken(Operator, "Modulo", "%")
    db.addToken(Operator, "At", "@")
    db.addToken(Operator, "Dot", ".")
    db.addToken(Operator, "Comma", ",")
    db.addToken(Operator, "Colon", ":")
    db.addToken(Operator, "Semicolon", ";")
    db.addToken(Operator, "Ternary", "?")
    db.addToken(Operator, "Sim", "~")
    db.addToken(Punctuation, "LBrace", "{")
    db.addToken(Punctuation, "RBrace", "}")
    db.addToken(Punctuation, "LParen", "(")
    db.addToken(Punctuation, "RParen", ")")
    db.addToken(Punctuation, "LBrack", "[")
    db.addToken(Punctuation, "RBrack", "]")
    return db
