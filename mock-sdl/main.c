#include "mock/display.h"
#include "mock/main_loop.h"

#include <emscripten.h>

int main(void) {
  mock_display_init();

  emscripten_set_main_loop(main_loop, 75, 1);

  return 0;
}
