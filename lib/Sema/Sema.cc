#include <Sema/Sema.hh>

namespace tlang {
Sema::Sema(CompilerInvocation &context) :
    context(context) {
}
void Sema::run() {
  completeTable();
  resolveTypes();
  resolveNames();
  inferTypes();
}
}
