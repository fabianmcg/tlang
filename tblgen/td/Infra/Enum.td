#ifndef ENUM_TD
#define ENUM_TD

class EnumMember<string I, code V = [{}]> {
  string identifier = I;
  code value = V;
}

class EM<string I, code V = [{}]> : EnumMember<I, V> {
}

class Enum<list<EnumMember> M, bit C = 0> {
  list<EnumMember> members = M;
  bit isClass = C;
}

#endif
