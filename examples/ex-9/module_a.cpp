#include <emscripten/bind.h>
#include <string>
#include <iostream>

void hello() {
  std::cout << "Hello, WebAssembly! (From C++)" << std::endl;
}

EMSCRIPTEN_BINDINGS(moduleA) {
  emscripten::function("js_hello", &hello);
}