#include <emscripten.h>
#include <iostream>

EM_JS(int, jsGetMouseX, (), {
  return window.mouseX;
});

EM_JS(int, jsGetMouseY, (), {
  return window.mouseY;
});

int g_MouseX = 0;
int g_MouseY = 0;

void MainLoop() {
  const int mouseX = jsGetMouseX();
  const int mouseY = jsGetMouseY();

  const bool changed = mouseX != g_MouseX || mouseY != g_MouseY;
  if (!changed) {
    return;
  }
  g_MouseX = mouseX;
  g_MouseY = mouseY;

  std::cout << "Mouse position: (" << g_MouseX << ", " << g_MouseY << ")" << std::endl;

  EM_ASM({
    const mousePosElem = document.getElementById('mouse-pos');
    mousePosElem.innerText = 'Mouse position: (' + $0 + ', ' + $1 + ')';
  }, g_MouseX, g_MouseY);
}

int main() {
  EM_ASM({
    window.mouseX = 0;
    window.mouseY = 0;

    window.addEventListener('mousemove', function(event) {
      window.mouseX = event.clientX;
      window.mouseY = event.clientY;
    });
  });

  emscripten_set_main_loop(MainLoop, 0, true);
  
  return 0;
}
