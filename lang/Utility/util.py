#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""


def getCxx(x):
    return str(x) if not hasattr(x, "cxx") else x.cxx()


def getParseStr(x, *args):
    return str(x) if not hasattr(x, "parseStr") else x.parseStr(*args)
