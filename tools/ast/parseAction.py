#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from enum import Enum

ActionKind = Enum("ActionKind", ["none", "set", "append"])

class ParseAction:
    def __init__(self, kind = ActionKind.none) -> None:
        self.kind = kind

    def genCodeQ(self):
        return False

    def genCode(self):
        return ""

class SetAction(ParseAction):
    def __init__(self, variable) -> None:
        super().__init__(kind=ActionKind.set)
        self.variable