#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

import argparse
from Conf.toks import makeLexer
from AST.confParser import makeAST
from Grammar.confParser import makeParser
from Grammar.grammar import GrammarGraph
from Utility.util import pathJoin


def parseArgs():
    parser = argparse.ArgumentParser(
        description="Lang generator",
        add_help=True,
        formatter_class=lambda prog: argparse.HelpFormatter(prog, max_help_position=60),
    )
    parser.add_argument(
        "feature",
        metavar="<feature>",
        default="all",
        nargs="?",
        choices=["all", "lexer", "parser", "ast"],
        help="Lang generator feature to generate",
    )
    parser.add_argument(
        "-o",
        "--output-dir",
        metavar="<output path>",
        type=str,
        default="tmp",
        help="Output directory path",
    )
    parser.add_argument(
        "-c",
        "--config-dir",
        metavar="<config path>",
        type=str,
        default="Conf",
        help="Config directory path",
    )
    parser.add_argument(
        "-t",
        "--template-dir",
        metavar="<template path>",
        type=str,
        default="Templates",
        help="Template directory path",
    )
    args = parser.parse_args()
    return args


def main():
    # args = parseArgs()
    # genAST = True if args.feature == "all" or args.feature == "ast" else False
    # genLex = True if args.feature == "all" or args.feature == "lexer" else False
    # genParser = True if args.feature == "all" or args.feature == "parser" else False
    tokens = makeLexer()
    # pathJoin(args.output_dir).mkdir(parents=True, exist_ok=True)
    # if genLex:
    #     tokens.generateLexer(args.output_dir, args.template_dir)
    # if genAST:
    #     ast = makeAST(args.config_dir)
    #     ast.generateASTNodes(args.output_dir, args.output_dir, str(pathJoin(args.template_dir, "header.hh.j2")))
    #     ast.generateRecursiveASTVisitor(args.output_dir, args.template_dir)
    # if genParser:
    #     grammar = makeParser(str(pathJoin(args.config_dir, "grammar.conf")), tokens)
    #     grammar.generateCxx(
    #         str(pathJoin(args.output_dir, "ParserHandler.cc")), str(pathJoin(args.template_dir, "Parser.cc.j2"))
    #     )
    grammar = makeParser(str(pathJoin("Conf/", "grammar.conf")), tokens)
    GrammarGraph(grammar).saveGraph("graph")


if __name__ == "__main__":
    main()
