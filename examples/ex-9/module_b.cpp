#include <emscripten/bind.h>
#include <string>
#include <iostream>

void hello_with_name(const std::string& name) {
  std::cout << "Hello, " << name << "! (From C++)" << std::endl;
}

EMSCRIPTEN_BINDINGS(moduleB) {
  emscripten::function("js_hello_with_name", &hello_with_name);
}