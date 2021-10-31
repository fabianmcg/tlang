#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""


class Node:
    def __init__(self, isParsable=False, *base):
        self.base = list(base)
        self.rules = []
        self.members = {}
        self.methods = []
        self.isParsable = isParsable

    def addRule(self, rule):
        self.rules.append(rule)

    def addMember(self, member):
        self.members.append(member)

    def addMethod(self, method):
        self.methods.append(method)


class ParsableNode(Node):
    def __init__(self, *base):
        super().__init__(isParsable=True, *base)
