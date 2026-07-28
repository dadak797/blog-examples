#include <iostream>
#include <emscripten.h>

extern "C" {
  void FindPrimes(int limit) {
    if (limit < 2) {
      std::cout << "No primes" << std::endl;
      return;
    }

    std::cout << "Primes up to " << limit << ": ";

    for (int i = 2; i <= limit; ++i) {
      bool isPrime = true;
      for (int j = 2; j * j <= i; ++j) {
        if (i % j == 0) {
          isPrime = false;
          break;
        }
      }

      if (isPrime) {
        std::cout << i << " ";
      }
    }

    std::cout << std::endl;
  }
}