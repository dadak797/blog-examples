#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <emscripten/bind.h>

std::vector<std::string> GetStrVector() {
  std::vector<std::string> arr = {"Hello", "WebAssembly"};
  return arr;
}

std::map<std::string, std::string> GetStrMap() {
  std::map<std::string, std::string> map = {
    {"Compiler", "Emscripten"},
    {"Age", "11"}
  };
  return map;
}

void LoadStrVector(const std::vector<std::string>& arr) {
  for (const auto& item : arr) {
    std::cout << item << std::endl;
  }
}

void LoadStrMap(const std::map<std::string, std::string>& map) {
  for (const auto& [key, value] : map) {
    std::cout << key << ": " << value << std::endl;
  }
}

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::register_vector<std::string>("StrVector");
  emscripten::register_map<std::string, std::string>("StrMap");
  emscripten::function("GetStrVector", &GetStrVector);
  emscripten::function("GetStrMap", &GetStrMap);
  emscripten::function("LoadStrVector", &LoadStrVector);
  emscripten::function("LoadStrMap", &LoadStrMap);
}