#include "mock/display.h"
#include "mock/main_loop.h"

#include <emscripten.h>
#include <stdio.h>

int main(void) {
  mock_display_init();

  puts("Controls:\n"
       "\n"
       "   E\n"
       " S D F   J K");

  emscripten_set_main_loop(main_loop, 90, 1);

  return 0;
}
