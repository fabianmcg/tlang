#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from subprocess import call


def format(filename):
    lc = ["clang-format-11", "-i", r'-style="{BasedOnStyle: llvm, ColumnLimit: 120}"', str(filename)]
    retcode = call(" ".join(lc), shell=True)
    return retcode
