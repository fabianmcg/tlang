#ifndef CHILDREN_TD
#define CHILDREN_TD

class ChildKind<bits<2> Kind> {
  bits<2> kind = Kind;
}

def Dynamic : ChildKind<0>;
def Static : ChildKind<1>;
def DynamicList : ChildKind<2>;
def StaticList : ChildKind<3>;

class Child<ChildKind Kind, code Type, bit Visit = 1> {
  ChildKind kind = Kind;
  code type = Type;
  bit visit = Visit;
}

#endif
