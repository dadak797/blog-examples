#include <iostream>

#ifdef __EMSCRIPTEN__
  #include <emscripten.h>
  #include <emscripten/html5.h>
#else
  #include <glad/gl.h>  // GLAD
#endif

// GLFW
#include <GLFW/glfw3.h>

// ImGui
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Initial background color
float g_BgColor[4] = {0.0f, 0.1f, 0.2f, 1.0f};

namespace {

#ifdef _WIN32
// Adjust the GUI scale to match the Windows display scale
void applyImGuiScale(float scale) {
  if (scale <= 0.0f) {
    scale = 1.0f;
  }

  ImGuiStyle style;
  ImGui::StyleColorsDark(&style);
  style.ScaleAllSizes(scale);
  style.FontScaleDpi = scale;
  ImGui::GetStyle() = style;
}

void windowContentScaleCallback(GLFWwindow*, float xScale, float yScale) {
  applyImGuiScale(xScale > 0.0f ? xScale : yScale);
}
#endif

// Called every frame to render the application
void renderFrame(GLFWwindow* window) {
  glfwPollEvents();

  // Render ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Test Window");
  ImGui::ColorEdit4("Background Color", g_BgColor);
  ImGui::End();

  ImGui::ShowDemoWindow();

  glClearColor(g_BgColor[0], g_BgColor[1], g_BgColor[2], g_BgColor[3]);
  glClear(GL_COLOR_BUFFER_BIT);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(window);
}

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
  glViewport(0, 0, width, height);
}

void shutdownGlfw(GLFWwindow* window) {
  if (window) {
    glfwDestroyWindow(window);
  }
  glfwTerminate();
}

void shutdownApplication(GLFWwindow* window) {
#ifdef __EMSCRIPTEN__
  emscripten_set_resize_callback(
    EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, false, nullptr);
  emscripten_set_fullscreenchange_callback(
    EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, false, nullptr);
  emscripten_set_wheel_callback(
    "#canvas", nullptr, false, nullptr);
#endif

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  shutdownGlfw(window);
}

#ifdef __EMSCRIPTEN__
// Called when the browser window is resized
EM_BOOL browserResizeCallback(
  int, const EmscriptenUiEvent* event, void* userData) {
  auto* window = static_cast<GLFWwindow*>(userData);

  if (event->windowInnerWidth > 0 && event->windowInnerHeight > 0) {
    glfwSetWindowSize(
      window, event->windowInnerWidth, event->windowInnerHeight);
  }

  return EM_FALSE;
}

// Called by emscripten_set_main_loop_arg
void browserMainLoop(void* argument) {
  auto* window = static_cast<GLFWwindow*>(argument);

  if (glfwWindowShouldClose(window)) {
    emscripten_cancel_main_loop();
    shutdownApplication(window);
    return;
  }

  renderFrame(window);
}
#endif

}  // namespace

int main() {
  int windowWidth = 800;
  int windowHeight = 600;

  std::cout << "Initialize GLFW" << std::endl;

  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

#ifdef __EMSCRIPTEN__
  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

  // Read the canvas CSS size and use it as the initial framebuffer size
  double canvasWidth;
  double canvasHeight;
  if (emscripten_get_element_css_size(
        "#canvas", &canvasWidth, &canvasHeight) == EMSCRIPTEN_RESULT_SUCCESS &&
      canvasWidth > 0.0 && canvasHeight > 0.0) {
    windowWidth = static_cast<int>(canvasWidth);
    windowHeight = static_cast<int>(canvasHeight);
  }
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
#ifdef _WIN32
  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
#endif
#endif

  std::cout << "Create GLFW window" << std::endl;

  GLFWwindow* window = glfwCreateWindow(
    windowWidth, windowHeight, "Emscripten Example-18", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  std::cout << "GLFW window created successfully" << std::endl;

#ifndef __EMSCRIPTEN__
  int version = gladLoadGL(glfwGetProcAddress);
  if (!version) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    shutdownGlfw(window);
    return -1;
  }
  std::cout << "GLAD initialized successfully, version: " << version << std::endl;
#endif

  const auto glVersion = glGetString(GL_VERSION);
  if (!glVersion) {
    std::cerr << "Failed to get OpenGL version" << std::endl;
    shutdownGlfw(window);
    return -1;
  }
  std::cout << "OpenGL version: "
            << reinterpret_cast<const char*>(glVersion) << std::endl;

  // ImGui initialization
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontDefaultVector();
  ImGui::StyleColorsDark();
  if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
    std::cerr << "Failed to initialize ImGui GLFW backend" << std::endl;
    ImGui::DestroyContext();
    shutdownGlfw(window);
    return -1;
  }

#ifdef _WIN32
  applyImGuiScale(ImGui_ImplGlfw_GetContentScaleForWindow(window));
  glfwSetWindowContentScaleCallback(window, windowContentScaleCallback);
#endif

#ifdef __EMSCRIPTEN__
  const char* glslVersion = "#version 300 es";
#else
  const char* glslVersion = "#version 330";
#endif
  if (!ImGui_ImplOpenGL3_Init(glslVersion)) {
    std::cerr << "Failed to initialize ImGui OpenGL backend" << std::endl;
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    shutdownGlfw(window);
    return -1;
  }

#ifdef __EMSCRIPTEN__
  ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");

  // Keep GLFW's HiDPI framebuffer while matching the canvas to the browser.
  emscripten_set_resize_callback(
    EMSCRIPTEN_EVENT_TARGET_WINDOW,
    window,
    false,
    browserResizeCallback);
#endif

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  int framebufferWidth;
  int framebufferHeight;
  glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
  framebufferSizeCallback(window, framebufferWidth, framebufferHeight);

  // Main loop
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(browserMainLoop, window, 0, true);
#else
  while (!glfwWindowShouldClose(window)) {
    renderFrame(window);
  }

  shutdownApplication(window);
#endif

  return 0;
}
