#include <iostream>

int main() {
  for (double i = 5; i < 7; i += 0.000000001) {
    auto dd = static_cast<int>(i);
    if (i - dd < 0) {
      std::cout << i << " " << dd << "\n";
    }
  }
  return 0;
}
