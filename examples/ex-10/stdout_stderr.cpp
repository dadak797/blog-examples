#include <emscripten/bind.h>
#include <string>
#include <iostream>

void print_to_stdout(const std::string& message) {
  std::cout << message << std::endl;
}

void print_to_stderr(const std::string& message) {
  std::cerr << message << std::endl;
}

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("js_print_to_stdout", &print_to_stdout);
  emscripten::function("js_print_to_stderr", &print_to_stderr);
}