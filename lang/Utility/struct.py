#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.type import Type
from Utility.variable import Variable
from Utility.dotDict import DotDict


class ClassParents:
    def __init__(self, *parents):
        self.parents = list(parents)

    def __str__(self) -> str:
        return "[" + ", ".join(list(map(str, self.parents))) + "]"

    __repr__ = __str__

    def __ilshift__(self, x):
        self.parents.extend(x.parents)
        return self

    def __iadd__(self, x):
        self.parents.append(x)
        return self

    def empty(self):
        return len(self.parents) == 0


class ClassMembers(DotDict):
    def __init__(self, *members):
        super().__init__({m.identifier: m for m in members})

    def __ilshift__(self, x):
        self.update(x)
        return self

    def __iadd__(self, x):
        self[x.identifier] = x
        return self

    def __str__(self) -> str:
        return str(list(self.values()))

    __repr__ = __str__

    def empty(self):
        return len(self) == 0


class ClassMethods(DotDict):
    def __init__(self, *methods):
        super().__init__({m.identifier: m for m in methods})

    def __ilshift__(self, x):
        self.update(x)
        return self

    def __iadd__(self, x):
        self[x.identifier] = x
        return self

    def __str__(self) -> str:
        return str(list(self.values()))

    __repr__ = __str__

    def empty(self):
        return len(self) == 0


class Class(Type):
    def __init__(
        self,
        typename,
        parents=None,
        members=None,
        methods=None,
    ):
        super().__init__(typename)
        self.parents = parents or ClassParents()
        self.members = members or ClassMembers()
        self.methods = methods or ClassMethods()

    def __str__(self) -> str:
        return "{} {{\n  Parents: {}\n  Members: {}\n  Methods: {}\n}};".format(
            self.T, self.parents, self.members, self.methods
        )

    __repr__ = __str__

    def __getattr__(self, k):
        if k in self.members:
            return self.members[k]
        elif k in self.methods:
            return self.methods[k]
        raise (KeyError("Unknown key: {}".format(k)))

    def __iadd__(self, x):
        if isinstance(x, Type):
            self.parents += x
        elif isinstance(x, Variable):
            self.members += x
        return self

    def __ilshift__(self, x):
        if isinstance(x, ClassParents):
            self.parents <<= x
        elif isinstance(x, ClassMembers):
            self.members <<= x
        elif isinstance(x, ClassMethods):
            self.methods <<= x
        return self

    def __enter__(self):
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        pass

    def shortRepr(self):
        return self.typename()
