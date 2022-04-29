#ifndef VISIBILITY_TD
#define VISIBILITY_TD

class Visibility<bits<2> I> {
  bits<2> id = I;
}
def Public : Visibility<0>;
def Protected : Visibility<1>;
def Private : Visibility<2>;

#endif
