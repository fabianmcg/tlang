#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from enum import Enum

ActionKind = Enum("ActionKind", ["token", "value", "node", "helper"])


class ParseAction:
    def __init__(self, kind=ActionKind.token):
        self.kind = kind


class TokenAction(ParseAction):
    def __init__(self):
        super().__init__(kind=ActionKind.token)


class ValueAction(ParseAction):
    def __init__(self):
        super().__init__(kind=ActionKind.value)


class NodeAction(ParseAction):
    def __init__(self):
        super().__init__(kind=ActionKind.node)


class HelperAction(ParseAction):
    def __init__(self):
        super().__init__(kind=ActionKind.helper)
