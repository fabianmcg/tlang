#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""
import textwrap
from subprocess import run


def format(filename):
    lc = ["clang-format-13", "-i", r'-style="{BasedOnStyle: llvm, ColumnLimit: 120}"', str(filename)]
    retcode = run(" ".join(lc), shell=True)
    return retcode


def formatStr(src):
    lc = ["clang-format-13", r'-style="{BasedOnStyle: llvm, ColumnLimit: 120}"']
    retcode = run(" ".join(lc), capture_output=True, text=True, shell=True, input=src)
    return retcode.stdout


def indentTxt(txt, indentation=0):
    return textwrap.indent(txt, " " * indentation, lambda x: True)


def getCxx(x):
    return str(x) if not hasattr(x, "cxx") else x.cxx()


def getParseStr(x, *args):
    return str(x) if not hasattr(x, "parseStr") else x.parseStr(*args)


def objetOrNone(x):
    if isinstance(x, (list, str)) or hasattr(x, "__len__"):
        return x if len(x) > 0 else None
    return x


def generateSectionComment(section):
    return "/*{}*/\n".format("{:*^76s}".format("{: ^30s}".format(section)))
