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

  EMSCRIPTEN_KEEPALIVE
  const char* get_hello() {
    return "Hello from C++!";
  }

  struct MyString {
    const char* str;
    size_t length;
  };

  EMSCRIPTEN_KEEPALIVE
  MyString get_hello_struct() {
    MyString myStr;
    myStr.str = "Hello from C++ struct!";
    myStr.length = strlen(myStr.str);
    return myStr;
  }
}