#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""


class EnumMember:
    def __init__(self, identifier, value=None, comment=None, string=None):
        self.identifier = identifier
        self.value = value
        self.comment = comment
        self.string = string or self.identifier

    def cxxEnum(self):
        return self.identifier, self.string

    def cxxString(self):
        return self.identifier, self.string
