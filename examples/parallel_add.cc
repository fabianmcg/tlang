#include <iostream>
#include <random>
#include <vector>

extern "C" void add(double*, double*, int);

void add_vec(std::vector<double> &x, std::vector<double> &y) {
  for (size_t i = 0; i < x.size(); ++i)
    x[i] += y[i];
}

int main() {
  size_t n = 1024 * 1024;
  std::vector<double> x(n, 0), y(n, 0);
  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0, 1.0);
  for (size_t i = 0; i < n; ++i) {
    x[i] = distribution(generator);
    y[i] = distribution(generator);
  }
  std::vector<double> xT = x, yT = y;
  add_vec(x, y);
  add(xT.data(), yT.data(), n);
  bool eq = true;
  for (size_t i = 0; i < n; ++i) {
    if (x[i] != xT[i]) {
      eq = false;
      break;
    }
  }
  if (eq)
    std::cerr << "Vecs are the same :)!" << std::endl;
  else {
    for (size_t i = 0; i < n; ++i) {
      std::cerr << i << ": " << x[i] << " " << xT[i] << std::endl;
    }
  }
  return 0;
}
