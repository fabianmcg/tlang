#ifndef __COMMON_EXCEPTION_HH__
#define __COMMON_EXCEPTION_HH__

#include <Common/Macros.hh>
#include <string>
#include <exception>

namespace __lang_np__ {
struct compiler_exception: public std::exception {
  std::string message { "Compiler Exception" };
  using std::exception::exception;
  compiler_exception(const std::string &message) :
      message(message) {
  }
  virtual const char* what() const noexcept {
    return message.data();
  }
};
} // namespace __lang_np__
#endif
