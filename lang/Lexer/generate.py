#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

import pathlib
from jinja2 import Template
from Lexer.db import LexerDB
from Lexer.token import *


def generateLexer(lexerDB: LexerDB, outputDir: str, inputDir="Templates"):
    tokens = lexerDB.tokens
    enum = {}
    switch = {}
    definitions = {k: v.rules[0] for k, v in lexerDB.definitions.items()}
    rules = {}
    for k, v in tokens.items():
        if isinstance(v, Keyword) or isinstance(v, Literal):
            enum[k] = v.rules[0]
            switch[k] = enum[k]
            rules['"{}"'.format(v.rules[0])] = k
        elif isinstance(v, Rule):
            enum[k] = " | ".join(v.rules)
            switch[k] = k
            for r in v.rules:
                rules["{}".format(r)] = k
        elif isinstance(v, Character):
            enum["{} = '{}'".format(k, v.rules[0])] = v.rules[0]
            switch[k] = v.rules[0]
            rules['"{}"'.format(v.rules[0])] = k

    with open(pathlib.Path(inputDir, "toks.hh.j2"), "r") as file:
        j2 = Template(file.read())
    with open(pathlib.Path(outputDir, "toks.hh"), "w") as file:
        print(j2.render({"enum": enum, "switch": switch}), file=file)
    with open(pathlib.Path(inputDir, "lex.yy.j2"), "r") as file:
        j2 = Template(file.read())
    with open(pathlib.Path(outputDir, "lex.yy"), "w") as file:
        print(j2.render({"definitions": definitions, "rules": rules}), file=file)
