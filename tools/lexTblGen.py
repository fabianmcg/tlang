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

namespace = "ttc"


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


def genEnum(r, v):
    if "value" in v:
        r = "{} = '{}'".format(r, v["value"])
    if isinstance(v["rule"], list):
        v = " | ".join(v["rule"])
    else:
        v = v["rule"]
    return r, v


def genTokens(tokDefs, inputDir, outputDir):
    enum = {}
    switch = {}
    for k, v in tokDefs.items():
        kk, vv = genEnum(k, v)
        enum[kk] = vv
        switch[k] = k if "value" not in v else v["value"]
    with open(pathlib.Path(inputDir, "toks.hh.j2"), "r") as file:
        j2 = Template(file.read())
    with open(pathlib.Path(outputDir, "toks.hh"), "w") as file:
        print(j2.render({"enum": enum, "switch": switch}), file=file)


def genLex(tokDefs, inputDir, outputDir):
    rules = {}
    for k, v in tokDefs["rules"].items():
        if isinstance(v["rule"], list):
            for r in v["rule"]:
                rules[r] = k
        else:
            rules['"' + v["rule"] + '"'] = k

    with open(pathlib.Path(inputDir, "lex.yy.j2"), "r") as file:
        j2 = Template(file.read())
    with open(pathlib.Path(outputDir, "lex.yy"), "w") as file:
        print(j2.render({"definitions": tokDefs["defs"], "rules": rules}), file=file)


def main():
    args = parseArgs()
    with open(pathlib.Path(args.i, "toks.yml"), "r") as file:
        rules = yaml.safe_load(file)
        genTokens(rules["rules"], args.i, args.o)
        genLex(rules, args.i, args.o)


if __name__ == "__main__":
    main()
