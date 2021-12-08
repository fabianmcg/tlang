#ifndef __IO_HH__
#define __IO_HH__

#include <fstream>
#include <iostream>
#include <string>

class unique_fstream {
protected:
  std::fstream file { };
  inline void close_nothrow() noexcept {
    if (file.is_open()) {
      try {
        file.close();
      } catch (std::ios_base::failure &exc) {
        std::cerr << std::string("Unable to close the file, the following error was given: ") + exc.what();
      }
    }
  }
public:
  unique_fstream() = default;
  unique_fstream(const std::string &name, std::ios_base::openmode mode = std::ios_base::out) {
    open(name, mode);
  }
  ~unique_fstream() {
    close_nothrow();
  }
  unique_fstream(unique_fstream &&other) = default;
  unique_fstream(const unique_fstream&) = delete;
  unique_fstream& operator=(unique_fstream &&other) {
    if (this != &other) {
      close();
      file = std::move(other.file);
    }
    return *this;
  }
  unique_fstream& operator=(const unique_fstream&) = delete;
  inline std::fstream& operator*() {
    return file;
  }
  inline std::fstream* operator->() {
    return &file;
  }
  inline bool valid() const {
    return file.good() && file.is_open();
  }
  inline void open(std::string file_name, std::ios_base::openmode open_mode) {
    if (file.is_open())
      close();
    file.open(file_name, open_mode);
    if (!file.is_open())
      throw std::ios_base::failure("Unable to open file: " + file_name);
  }
  inline void close() {
    if (file.is_open())
      file.close();
  }
  static unique_fstream open_ostream(const std::string &name, std::ios_base::openmode mode = std::ios_base::out) {
    return unique_fstream { name, mode | std::ios_base::out };
  }
  static unique_fstream open_istream(const std::string &name, std::ios_base::openmode mode = std::ios_base::in) {
    return unique_fstream { name, mode | std::ios_base::in };
  }
};
#endif
