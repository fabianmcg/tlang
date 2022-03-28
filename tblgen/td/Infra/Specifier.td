#ifndef SPECIFIER_TD
#define SPECIFIER_TD

class DeclSpecifier<bits<2> Kind> {
  bits<2> kind = Kind;
}

def Default : DeclSpecifier<0>;
def Deleted : DeclSpecifier<1>;
def UserDefined : DeclSpecifier<2>;

#endif
