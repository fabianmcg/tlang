#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""
from Utility.struct import Class


class Node(Class):
    def __init__(self, typename, accessors=True, loc=False):
        super().__init__(typename)
        self.generateGetSet = accessors
        self.hasLocation = loc
