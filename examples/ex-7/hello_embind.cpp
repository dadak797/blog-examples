#include <emscripten.h>
#include <emscripten/bind.h>
#include <string>
#include <iostream>

void hello() { 
  std::cout << "Hello, WebAssembly! (From C++)" << std::endl;
}

void hello(const std::string& name) {
  std::cout << "Hello, " << name << "! (From C++)" << std::endl;
}

void hello_with_number(int number) {
  std::cout << "Hello, " << number << "! (From C++)" << std::endl;
}

std::string get_hello_with_name(const std::string& name) {
  std::string message = "Hello, " + name + "! (From C++)";
  return message;
}

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("js_hello", emscripten::select_overload<void()>(&hello));
  emscripten::function("js_hello", emscripten::select_overload<void(const std::string&)>(&hello));
  emscripten::function("js_hello_with_number", &hello_with_number);
  emscripten::function("js_get_hello_with_name", &get_hello_with_name);
}