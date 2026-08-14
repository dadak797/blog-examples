#include <iostream>
#include <emscripten.h>

int main() {
  EM_ASM({
    console.log("Hello, WebAssembly! (From C++ via JavaScript)");
  });

  int c_number = 100;
  EM_ASM({
    const js_number = $0; 
    console.log("Hello, " + js_number + "! (From C++ with number via JavaScript)");
  }, c_number);

  const char* c_name = "WebAssembly";
  EM_ASM({
    const js_name_ptr = $0;
    const js_name = UTF8ToString(js_name_ptr);
    console.log("Hello, " + js_name + "! (From C++ with string via JavaScript)");
  }, c_name);

  return 0;
}