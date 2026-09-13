#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>

void onLoadWget(const char* filename) {
  std::cout << "File downloaded: " << filename << std::endl;
  std::ifstream ifs(filename);
  std::cout << "File content:" << std::endl;
  std::string line;
  while (std::getline(ifs, line)) {
    std::cout << line << std::endl;
  }
  ifs.close();
}

void onErrorWget(const char* filename) {
  std::cerr << "Error downloading file: " << filename << std::endl;
}

void GetFile(const std::string& url, const std::string& filename) {
  emscripten_async_wget(url.c_str(), filename.c_str(), onLoadWget, onErrorWget);
}

void onLoadWgetData(void* userdata, void* data, int size) {
  char* filename = static_cast<char*>(userdata);
  std::cout << "Data downloaded to memory for file: " << filename << std::endl;
  char* buffer = static_cast<char*>(data);
  std::string content(buffer, size);
  std::cout << "Data content:" << std::endl;
  std::cout << content << std::endl;
  delete[] filename;
}

void onErrorWgetData(void* userdata) {
  char* filename = static_cast<char*>(userdata);
  std::cerr << "Error downloading data: " << filename << std::endl;
  delete[] filename;
}

void GetFileData(const std::string& url, const std::string& filename) {
  char* userData = new char[filename.size() + 1];
  std::strcpy(userData, filename.c_str());
  emscripten_async_wget_data(url.c_str(), static_cast<void*>(userData), onLoadWgetData, onErrorWgetData);
}

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("getFile", &GetFile);
  emscripten::function("getFileData", &GetFileData);
}
