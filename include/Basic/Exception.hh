#ifndef BASIC_EXCEPTION_HH
#define BASIC_EXCEPTION_HH

#include <string>
#include <exception>

namespace tlang {
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
