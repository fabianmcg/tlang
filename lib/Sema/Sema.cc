#include <Sema/Sema.hh>

namespace tlang {
Sema::Sema(ASTContext &context) :
    context(context) {
}
void Sema::run() {
  completeTable();
  resolveTypes();
  resolveNames();
  inferTypes();
}
void Sema::inferTypes() {

}
}
