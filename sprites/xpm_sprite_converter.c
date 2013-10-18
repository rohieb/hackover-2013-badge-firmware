#ifndef XPM_FILE
#error "XPM_FILE undefined"
#endif

#ifndef XPM_NAME
#error "XPM_NAME undefined"
#endif

#define STRINGIFY_I(x) #x
#define STRINGIFY(x) STRINGIFY_I(x)

#include STRINGIFY(XPM_FILE)

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  int width, height, color_count, foo;
  char black = '\0';
  uint8_t *data;

  if(4 != sscanf(XPM_NAME[0], "%d %d %d %d", &width, &height, &color_count, &foo)) {
    fputs("Fehler beim Parsen der Dimensionen\n", stderr);
    return -1;
  }

  for(int i = 0; i < color_count; ++i) {
    size_t len = strlen((char*) XPM_NAME[i + 1]);
    if(len > 7 && strcmp((char*) XPM_NAME[i + 1] + len - 7, "#000000") == 0) {
      black = XPM_NAME[i + 1][0];
      break;
    }
  }

  if(!black) {
    fputs("Konnte Schwarz nicht identifizieren.\n", stderr);
    return -1;
  }

  data = calloc(width * height / 8 + 1, 1);

  printf("{ %d, %d, (uint8_t const *) \"", width, height);

  if(data) {
    int y, x;
    int i = 0, j;

    for(x = 0; x < width; ++x) {
      for(y = 0; y < height; ++y) {
	data[i / 8] |= (XPM_NAME[y + 1 + color_count][x] == black) << i % 8;
	++i;
      }
    }

    for(j = 0; j < i / 8 + (i % 8 != 0); ++j) {
      printf("\\x%02x", data[j]);
    }
    puts("\" }");
  }

  free(data);
}
