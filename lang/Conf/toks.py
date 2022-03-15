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
    #   Language constructs
    # ******************************************************************************
    db.addToken(Keyword, "Typeof")
    db.addToken(Keyword, "Function", "fn")
    db.addToken(Keyword, "Struct")
    db.addToken(Keyword, "Let")
    db.addToken(Keyword, "Enum")
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
    db.addToken(Keyword, "Constexpr")
    db.addToken(Keyword, "Alignment")
    db.addToken(Keyword, "Namespace")
    db.addToken(Keyword, "This")
    db.addToken(Keyword, "Allocate")
    db.addToken(Keyword, "Parallel")
    db.addToken(Keyword, "Loop")
    db.addToken(Keyword, "Leading")
    db.addToken(Keyword, "Single")
    db.addToken(Keyword, "Sync")
    db.addToken(Keyword, "Reduce")
    db.addToken(Keyword, "Safe")
    db.addToken(Keyword, "Nowait")
    db.addToken(Keyword, "Private")
    db.addToken(Keyword, "Shared")
    db.addToken(Keyword, "Atomic")
    db.addToken(Keyword, "Reduce")
    db.addToken(Keyword, "In")
    db.addToken(Keyword, "Extern")
    # ******************************************************************************
    #   Fundamental types
    # *****************************************************************************
    db.addToken(Keyword, "Bool")
    db.addToken(Keyword, "Int")
    db.addToken(Keyword, "I8")
    db.addToken(Keyword, "I16")
    db.addToken(Keyword, "I32")
    db.addToken(Keyword, "I64")
    db.addToken(Keyword, "Uint")
    db.addToken(Keyword, "U8")
    db.addToken(Keyword, "U16")
    db.addToken(Keyword, "U32")
    db.addToken(Keyword, "U64")
    db.addToken(Keyword, "Float")
    db.addToken(Keyword, "F8")
    db.addToken(Keyword, "F16")
    db.addToken(Keyword, "F32")
    db.addToken(Keyword, "F64")
    db.addToken(Keyword, "Address")
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
    db.addToken(Operator, "Ellipsis", "...")
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
    db.addToken(Punctuation, "LArrow", "<-")
    db.addToken(Punctuation, "RArrow", "->")
    return db
