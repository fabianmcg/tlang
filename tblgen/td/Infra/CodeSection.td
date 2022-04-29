#ifndef CODE_SECTION_TD
#define CODE_SECTION_TD

include "Infra/Visibility.td"

class CodeSectionLocation<bit Id> {
  bit id = Id;
}

def Header : CodeSectionLocation<0>;
def Source : CodeSectionLocation<1>;

class CodeSection<CodeSectionLocation L, code S> {
  CodeSectionLocation location = L;
  code source = S;
}

class ClassSection<CodeSectionLocation L, Visibility V, code S> : CodeSection<L, S> {
  Visibility visibility = V;
}

class Prologue<CodeSectionLocation L, code S, bit I = 0> : CodeSection<L, S> {
  bit insideNamespace = I;
}

class Epilogue<CodeSectionLocation L, code S, bit I = 0> : CodeSection<L, S> {
  bit insideNamespace = I;
}

#endif
