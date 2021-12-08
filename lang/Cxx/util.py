#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""


class CxxList(list):
    def __str__(self) -> str:
        return self.parseStr()

    def __repr__(self) -> str:
        return str(self)

    def empty(self):
        return len(self) == 0

    def notEmpty(self):
        return not self.empty()

    def parseStr(self, indentation=0):
        return ""
