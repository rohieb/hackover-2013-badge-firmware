#include "font.h"
#include "sprite.h"

#include <drivers/fatfs/ff.h>

static uint8_t badge_framebuffer_render_char_with_fd(badge_framebuffer *fb, int8_t pos_x, int8_t pos_y, char c, FIL *fd) {
  UINT readbytes;
  uint8_t buffer[BADGE_FONT_WIDTH];

  if(c == 32) {
    // intentionally left empty.
  } else if(c >= 32 &&
     FR_OK == f_lseek(fd, (c - 33) * BADGE_FONT_WIDTH) &&
     FR_OK == f_read(fd, buffer, sizeof(buffer), &readbytes)) {
    badge_sprite sp = { BADGE_FONT_WIDTH, BADGE_FONT_HEIGHT, buffer };
    badge_framebuffer_blt(fb, pos_x, pos_y, &sp, 0);
  } else {
    return -1;
  }

  return 0;
}

static FRESULT open_font_file(FIL *fd) {
  return f_open(fd, "font.dat", FA_OPEN_EXISTING | FA_READ);
}

uint8_t badge_framebuffer_render_char(badge_framebuffer *fb, int8_t pos_x, int8_t pos_y, char c) {
  FIL fd;

  if(FR_OK == open_font_file(&fd)) {
    badge_framebuffer_render_char_with_fd(fb, pos_x, pos_y, c, &fd);
    f_close(&fd);
    return 0;
  }

  return -1;
}

uint8_t badge_framebuffer_render_text(badge_framebuffer *fb, int8_t pos_x, int8_t pos_y, char const *text) {
  uint8_t count = 0;
  FIL fd;

  if(FR_OK == open_font_file(&fd)) {
    while(*text) {
      if(0 != badge_framebuffer_render_char_with_fd(fb, pos_x, pos_y, *text, &fd)) {
        break;
      }

      ++count;
      ++text;
      pos_x += BADGE_FONT_WIDTH;
    }

    f_close(&fd);
  }

  return count;
}

static uint8_t badge_framebuffer_render_number_with_fd(badge_framebuffer *fb, int8_t *pos_x, int8_t pos_y, uint8_t number, FIL *fd) {
  if(number > 10) {
    badge_framebuffer_render_number_with_fd(fb, pos_x, pos_y, number / 10, fd);
  }

  uint8_t err = badge_framebuffer_render_char_with_fd(fb, *pos_x, pos_y, '0' + number % 10, fd);

  if(err != 0) return err;

  *pos_x += BADGE_FONT_WIDTH;
  return 0;
}

uint8_t badge_framebuffer_render_number(badge_framebuffer *fb, int8_t pos_x, int8_t pos_y, uint8_t number) {
  FIL fd;

  if(FR_OK == open_font_file(&fd)) {
    uint8_t err = badge_framebuffer_render_number_with_fd(fb, &pos_x, pos_y, number, &fd);

    if(err != 0) {
      return err;
    }
  }

  return 0;
}
