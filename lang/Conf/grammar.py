#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Lang.db import RTok as T
from Conf.nodes import ast as A
from Lang.rule import *


A.Function <<= T.LParen + T.RParen
