#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Nov Sun 7 08:01:00 2021

@author: fabian
"""


class DotDict(dict):
    def __init__(self, *args, **kwargs):
        dict.__init__(self, *args, **kwargs)

    def __getattr__(self, key):
        return self.__getitem__(key)


class ApplyDotDict(dict):
    def __init__(self, function, *args, **kwargs):
        self.function = function
        dict.__init__(self, *args, **kwargs)

    def __getattr__(self, key):
        return self.function(self.__getitem__(key))


class DotDictWrapper:
    def __init__(self, Dict, action=lambda x: x):
        self.Dict = Dict
        self.function = action

    def __str__(self) -> str:
        return str(self.Dict)

    __repr__ = __str__

    def __getattr__(self, key):
        return self.function(self.Dict.__getitem__(key))

    def get(self):
        return self.Dict
