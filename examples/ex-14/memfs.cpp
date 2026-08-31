#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader/tiny_obj_loader.h"
#include "nlohmann/json.hpp"

#include <optional>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <limits>
#include <utility>
#include <emscripten.h>
#include <emscripten/bind.h>

using json = nlohmann::json;
namespace fs = std::filesystem;

// Stores data loaded from an OBJ file
struct ObjFile {
  std::string filename;
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
};

std::vector<ObjFile> g_ObjFiles;

// Lists files and directories at a specific MEMFS path
void PrintFiles(const std::string& path) {
  for (const auto& entry : fs::directory_iterator(path)) {
    if (entry.is_directory()) {
      std::cout << "[DIR ] ";
    }
    else if (entry.is_regular_file()) {
      std::cout << "[FILE] ";
    }
    std::cout << entry.path() << std::endl;
  }
}

// Loads OBJ files from a JSON string of file names and returns their summaries
std::optional<std::string> LoadObjFiles(const std::string& filenames) {
  std::cout << "=== Before loading OBJ files ===" << std::endl;
  PrintFiles("/");  // Inspect files and directories in the MEMFS root

  g_ObjFiles.clear();

  json files = json::parse(filenames);
  if (!files.is_array()) {
    std::cerr << "Input must be a JSON array of file names." << std::endl;
    return std::nullopt;
  }

  json summary = json::array();  // JSON array containing the summaries

  for (const auto& file : files) {
    if (!file.is_string()) {
      std::cerr << "Each file name must be a string." << std::endl;
      return std::nullopt;
    }
    std::string filename = file.get<std::string>();

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // Load vertex attributes, shapes, and materials from the OBJ file
    bool ok = tinyobj::LoadObj(
      &attrib,
      &shapes,
      &materials,
      &warn,
      &err,
      filename.c_str(),
      nullptr,
      false  // Set to true to triangulate every face
    );

    fs::remove(filename.c_str());  // Remove the OBJ file from MEMFS after reading it

    if (!warn.empty()) {
      std::cout << "Warning: " << warn << std::endl;
    }
    if (!err.empty()) {
      std::cerr << "Error: " << err << std::endl;
      continue;
    }
    if (!ok) {
      std::cerr << "Failed to load/parse .obj file." << std::endl;
      continue;
    }

    for (size_t i = 0; i < shapes.size(); ++i) {
      uint32_t triangleFaceCount = 0;    // Number of triangular faces
      uint32_t quadFaceCount = 0;        // Number of quad faces
      uint32_t ngonFaceCount = 0;        // Number of n-gon faces
      uint32_t renderTriangleCount = 0;  // Number of triangles after triangulation

      for (const auto vertexCount : shapes[i].mesh.num_face_vertices) {
        if (vertexCount == 3) {
          ++triangleFaceCount;
        } else if (vertexCount == 4) {
          ++quadFaceCount;
        } else if (vertexCount > 4) {
          ++ngonFaceCount;
        }

        if (vertexCount >= 3) {
          renderTriangleCount += vertexCount - 2;
        }
      }

      summary.push_back({
        {"name", shapes[i].name},
        {"vertices", attrib.vertices.size() / 3},
        {"faces", shapes[i].mesh.num_face_vertices.size()},
        {"triangleFaces", triangleFaceCount},
        {"quadFaces", quadFaceCount},
        {"ngonFaces", ngonFaceCount},
        {"renderTriangles", renderTriangleCount}
      });
    }

    // Store the file name, vertex attributes, and shapes in g_ObjFiles
    g_ObjFiles.push_back({
      filename,
      std::move(attrib),
      std::move(shapes)
    });
  }

  std::cout << "=== After loading OBJ files ===" << std::endl;
  PrintFiles("/");  // Verify that the temporary OBJ files were removed from MEMFS

  return summary.dump();  // Serialize and return the summaries as a JSON string
}

// Merges the objects in g_ObjFiles, writes an OBJ file, and downloads it
void MergeAndDownloadObjFiles() {
  constexpr const char* MERGED_OBJ_FILE = "/merged.obj";  // Output file path in MEMFS
  std::ofstream output(MERGED_OBJ_FILE);
  if (!output) {
    std::cerr << "Failed to create merged OBJ file." << std::endl;
    return;
  }

  output << std::setprecision(
    std::numeric_limits<tinyobj::real_t>::max_digits10
  );
  output << "# Merged OBJ file\n";

  std::size_t vertexOffset = 0;
  std::size_t normalOffset = 0;
  std::size_t texcoordOffset = 0;

  for (std::size_t fileIndex = 0;
       fileIndex < g_ObjFiles.size();
       ++fileIndex) {
    const auto& loadedObj = g_ObjFiles[fileIndex];
    const auto& attrib = loadedObj.attrib;

    output << "\n# Source: " << loadedObj.filename << '\n';

    // Write vertex positions
    for (std::size_t i = 0; i + 2 < attrib.vertices.size(); i += 3) {
      output << "v "
             << attrib.vertices[i] << ' '
             << attrib.vertices[i + 1] << ' '
             << attrib.vertices[i + 2] << '\n';
    }

    // Write vertex normals
    for (std::size_t i = 0; i + 2 < attrib.normals.size(); i += 3) {
      output << "vn "
             << attrib.normals[i] << ' '
             << attrib.normals[i + 1] << ' '
             << attrib.normals[i + 2] << '\n';
    }

    // Write vertex texture coordinates
    for (std::size_t i = 0; i + 1 < attrib.texcoords.size(); i += 2) {
      output << "vt "
             << attrib.texcoords[i] << ' '
             << attrib.texcoords[i + 1] << '\n';
    }

    // Write the vertex indices for each face
    for (std::size_t shapeIndex = 0;
         shapeIndex < loadedObj.shapes.size();
         ++shapeIndex) {
      const auto& shape = loadedObj.shapes[shapeIndex];
      output << "o " << fileIndex + 1 << '_' << shapeIndex + 1;
      if (!shape.name.empty()) {
        output << '_' << shape.name;
      }
      output << '\n';

      // Apply offsets because attributes from multiple objects share global arrays
      std::size_t indexOffset = 0;
      for (const auto faceVertexCount : shape.mesh.num_face_vertices) {
        if (indexOffset + faceVertexCount > shape.mesh.indices.size()) {
          std::cerr << "Invalid face indices in " << loadedObj.filename
                    << std::endl;
          output.close();
          fs::remove(MERGED_OBJ_FILE);
          return;
        }

        output << 'f';
        for (std::size_t i = 0; i < faceVertexCount; ++i) {
          const auto& index = shape.mesh.indices[indexOffset + i];
          if (index.vertex_index < 0) {
            std::cerr << "Missing vertex index in " << loadedObj.filename
                      << std::endl;
            output.close();
            fs::remove(MERGED_OBJ_FILE);
            return;
          }

          const auto vertexIndex =
            static_cast<std::size_t>(index.vertex_index) + vertexOffset + 1;

          output << ' ' << vertexIndex;

          const bool hasTexcoord = index.texcoord_index >= 0;
          const bool hasNormal = index.normal_index >= 0;
          if (hasTexcoord || hasNormal) {
            output << '/';
            if (hasTexcoord) {
              output << static_cast<std::size_t>(index.texcoord_index)
                        + texcoordOffset + 1;
            }
            if (hasNormal) {
              output << '/'
                     << static_cast<std::size_t>(index.normal_index)
                          + normalOffset + 1;
            }
          }
        }
        output << '\n';
        indexOffset += faceVertexCount;
      }
    }

    vertexOffset += attrib.vertices.size() / 3;
    normalOffset += attrib.normals.size() / 3;
    texcoordOffset += attrib.texcoords.size() / 2;
  }

  output.close();
  if (!output) {
    std::cerr << "Failed to write merged OBJ file." << std::endl;
    fs::remove(MERGED_OBJ_FILE);
    return;
  }

  std::cout << "Merged " << g_ObjFiles.size()
            << " OBJ files into " << MERGED_OBJ_FILE << std::endl;

  // Read the MEMFS file and download it using the DOM
  EM_ASM({
    // Convert the MERGED_OBJ_FILE pointer to a JavaScript string
    const path = UTF8ToString($0);
    
    // Read the MEMFS file into a Uint8Array
    const data = Module.FS.readFile(path);
    
    // Create a Blob containing the file data
    const blob = new Blob([data], { type: "text/plain" });
    
    // Create a temporary URL for the Blob
    const url = URL.createObjectURL(blob);
    
    // Create an anchor element for the download
    const anchor = document.createElement("a");
    anchor.href = url;
    anchor.download = "merged.obj";
    document.body.appendChild(anchor);
    
    // Start the download
    anchor.click();
    anchor.remove();
    
    // Revoke the temporary Blob URL
    setTimeout(() => URL.revokeObjectURL(url), 0);
  }, MERGED_OBJ_FILE);

  // Remove the merged file from MEMFS after starting the download
  fs::remove(MERGED_OBJ_FILE);
}

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("loadObjFile", &LoadObjFiles);
  emscripten::register_optional<std::string>();
  emscripten::function("mergeAndDownloadObjFiles", &MergeAndDownloadObjFiles);
}
