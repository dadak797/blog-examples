#include <cstddef>
#include <vector>
#include <iostream>
#include <emscripten/bind.h>

std::vector<float> g_Vertices = {
  1.0f, 2.0f, 3.0f, 1.0f, 0.0f, 0.0f,  // Vertex 1: position (1,2,3), color (1,0,0)
  4.0f, 5.0f, 6.0f, 0.0f, 1.0f, 0.0f,  // Vertex 2: position (4,5,6), color (0,1,0)
  7.0f, 8.0f, 9.0f, 0.0f, 0.0f, 1.0f   // Vertex 3: position (7,8,9), color (0,0,1)
};

uintptr_t GetVertexData() {
  return reinterpret_cast<uintptr_t>(g_Vertices.data());
}

std::size_t GetVertexDataCount() {
  return g_Vertices.size();
}

void LoadVertexData(uintptr_t ptr, std::size_t size) {
  float* data = reinterpret_cast<float*>(ptr);
  for (std::size_t i = 0; i < size; ++i) {
    std::cout << data[i] << " ";
    if ((i + 1) % 6 == 0) { // Print a newline after every 6 floats (one vertex)
      std::cout << std::endl;
    }
  }
}

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("getVertexData", &GetVertexData);
  emscripten::function("getVertexDataCount", &GetVertexDataCount);
  emscripten::function("loadVertexData", &LoadVertexData);
}