#include <Driver/Driver.hh>

int main(int argc, char **argv) {
  tlang::driver::Driver driver;
  return driver.run(argc, argv);
}
