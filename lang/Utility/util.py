#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from collections.abc import Iterable


def makeList(x):
    return x if isinstance(x, list) else (list(x) if isinstance(x, Iterable) and not isinstance(x, str) else [x])


def makeListVariadic(*x):
    return makeList(x[0]) if len(x) == 1 else list(x)


def getShort(x):
    return str(x) if not hasattr(x, "shortRepr") else x.shortRepr()


def getCxx(x):
    return str(x) if not hasattr(x, "cxx") else x.cxx()
