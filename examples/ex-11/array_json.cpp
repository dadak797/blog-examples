#include "nlohmann/json.hpp"
#include <string>
#include <iostream>
#include <emscripten/bind.h>

using json = nlohmann::json;

std::string GetJsonStr(bool isArray) {
  if (isArray) {
    json j = json::array({"Hello", "WebAssembly", "and", "JSON"});
    return j.dump();
  } else {
    json j = json::object({{"Compiler", "Emscripten"}, {"Age", 11}, {"Language", "C++"}});
    return j.dump();
  }
}

void LoadJsonStr(const std::string& jsonStr) {
  auto j = json::parse(jsonStr);

  if (j.is_array()) {
    for (const auto& item : j) {
      std::cout << item << std::endl;
    }
    return;
  }

  if (j.is_object()) {
    for (const auto& [key, value] : j.items()) {
      std::cout << key << ": " << value << std::endl;
    }
    return;
  }
}

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("getJsonStr", &GetJsonStr);
  emscripten::function("loadJsonStr", &LoadJsonStr);
}