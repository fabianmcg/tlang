#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from .. import Node

astNodes = {}


def addNode(kind, identifier, *rules):
    astNodes[identifier] = kind(identifier, *rules)