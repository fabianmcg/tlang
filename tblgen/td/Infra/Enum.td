#ifndef ENUM_TD
#define ENUM_TD

class EnumMember<string I, code V = [{}], code S = [{}]> {
  string identifier = I;
  code value = V;
  code stringRepresentation = S;
}

class EM<string I, code V = [{}], code S = [{}]> : EnumMember<I, V, S> {
}

class Enum<list<EnumMember> M, bit C = 0> {
  list<EnumMember> members = M;
  bit isClass = C;
}

#endif
