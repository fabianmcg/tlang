#ifndef __EXCEPTION_HH__
#define __EXCEPTION_HH__

#include <exception>

namespace ttc {
struct exception : public std::exception {
  using std::exception::exception;
  virtual const char* what() const noexcept = 0;
};
}
#endif
