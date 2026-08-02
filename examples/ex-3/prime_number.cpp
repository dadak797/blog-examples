#ifdef __EMSCRIPTEN__
#include <emscripten.h>
extern "C" {
  EMSCRIPTEN_KEEPALIVE
#endif // __EMSCRIPTEN__
  int CountPrimes(int limit) {
    if (limit < 2) return 0;
    int count = 0;
    for (int i = 2; i <= limit; ++i) {
      bool isPrime = true;
      for (int j = 2; j * j <= i; ++j) {
        if (i % j == 0) {
          isPrime = false;
          break;
        }
      }
      if (isPrime) ++count;
    }
    return count;
  }
#ifdef __EMSCRIPTEN__
}
#endif // __EMSCRIPTEN__
