#ifndef VARIABLE_TD
#define VARIABLE_TD

include "Infra/Visibility.td"

class Variable<code T, code I, Visibility V, bit E = 0> {
  code type = T;
  code init = I;
  Visibility visibility = V;
  bit isEnum = E;
}

class Var<code T, code I = [{}], Visibility V = Protected, bit E = 0> : Variable<T, I, V, E> {
}

class EVar<code T, code I = [{}], Visibility V = Protected> : Variable<T, I, V, 1> {
}

#endif
