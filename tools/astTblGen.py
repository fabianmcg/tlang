#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Fri 29 08:35:00 2021

@author: fabian
"""

import argparse
import yaml
import pathlib
from jinja2 import Template


def parseArgs(argsString=""):
    parser = argparse.ArgumentParser(
        description="tblgen",
        add_help=True,
        formatter_class=lambda prog: argparse.HelpFormatter(prog, max_help_position=60),
    )
    parser.add_argument(
        "-i",
        metavar="<input dir>",
        type=str,
        default="./",
        help="input dir name",
    )
    parser.add_argument(
        "-o",
        metavar="<output dir>",
        type=str,
        default="./",
        help="output dir name",
    )
    if len(argsString) == 0:
        args = parser.parse_args()
    else:
        args = parser.parse_args(argsString.split(" "))
    return args


def genClasses(tokDefs, inputDir, outputDir):
    print(tokDefs)

    # with open(pathlib.Path(inputDir, "lex.yy.j2"), "r") as file:
    #     j2 = Template(file.read())
    # with open(pathlib.Path(outputDir, "lex.yy"), "w") as file:
    #     print(j2.render({"definitions": tokDefs["defs"], "rules": rules}), file=file)


def main():
    args = parseArgs()
    with open(pathlib.Path(args.i, "ast.yml"), "r") as file:
        astDef = yaml.safe_load(file)
        genClasses(astDef, args.i, args.o)


if __name__ == "__main__":
    main()
