#include <iostream>
#include <emscripten.h>

extern "C" {
  EMSCRIPTEN_KEEPALIVE
  void hello() {
    std::cout << "Hello, WebAssembly! (From C++)" << std::endl;
  }

  EMSCRIPTEN_KEEPALIVE
  void hello_with_number(int number) {
    std::cout << "Hello, " << number << "! (From C++)" << std::endl;
  }

  EMSCRIPTEN_KEEPALIVE
  void hello_with_name(const char* name) {
    std::cout << "Hello, " << name << "! (From C++)" << std::endl;
  }
}