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
from Lexer.token import Character, Keyword, Literal, Rule
from Lexer.db import LexerDB


def makeLexerDB():
    db = LexerDB()
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
    db.addToken(Keyword, "Computation_context")
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
    db.addToken(Keyword, "__builtin__")
    db.addToken(Keyword, "Typeof")
    db.addToken(Keyword, "Function")
    db.addToken(Keyword, "Struct")
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
    db.addToken(Rule, "IntLiteral", r"{NZ}{D}*")
    db.addToken(Rule, "FloatLiteral", r"{D}+{E}", r'{D}*"."{D}+{E}?')
    db.addToken(Rule, "Identifier", r"{L}{A}*")
    # ******************************************************************************
    #
    #   Operands
    #
    # ******************************************************************************
    db.addToken(Literal, "And", "&&")
    db.addToken(Literal, "Or", "||")
    db.addToken(Literal, "Equal", "==")
    db.addToken(Literal, "NEQ", "!=")
    db.addToken(Literal, "LEQ", "<=")
    db.addToken(Literal, "GEQ", ">=")
    db.addToken(Literal, "LShift", "<<")
    db.addToken(Literal, "RShift", ">>")
    db.addToken(Literal, "Pow", "^^")
    db.addToken(Literal, "Namespace", "::")
    # ******************************************************************************
    #   Characters
    # ******************************************************************************
    db.addToken(Character, "Not", "!")
    db.addToken(Character, "Band", "&")
    db.addToken(Character, "Bor", "|")
    db.addToken(Character, "Less", "<")
    db.addToken(Character, "Greater", ">")
    db.addToken(Character, "Assign", "=")
    db.addToken(Character, "Plus", "+")
    db.addToken(Character, "Minus", "-")
    db.addToken(Character, "Multiply", "*")
    db.addToken(Character, "Divide", "/")
    db.addToken(Character, "At", "@")
    db.addToken(Character, "Dot", ".")
    db.addToken(Character, "Comma", ",")
    db.addToken(Character, "Colon", ":")
    db.addToken(Character, "Semicolon", ";")
    db.addToken(Character, "Ternary", "?")
    db.addToken(Character, "LBrace", "{")
    db.addToken(Character, "RBrace", "}")
    db.addToken(Character, "LParen", "(")
    db.addToken(Character, "RParen", ")")
    db.addToken(Character, "LBrack", "[")
    db.addToken(Character, "RBrack", "]")
    return db
