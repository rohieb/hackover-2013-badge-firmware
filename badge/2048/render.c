#include <stdint.h>

#include "../ui/event.h"
#include "../ui/font.h"

#include "render.h"

static uint16_t const number_glyphs[][FIELD_SIZE] = {
  {
    0b0000000000000,
    0b0000000000000,
    0b0000000000000,
    0b0000000000000,
    0b0000000000000,
    0b0000000000000,
    0b0000000000000,
    0b0000000000000,
    0b0000000000000,
    0b0000000000000,
    0b0000000000000,
    0b0000000000000,
    0b0000000000000
  },
  {
    0b0000000000000,
    0b0000111110000,
    0b0000111110000,
    0b0000000110000,
    0b0000000110000,
    0b0000000110000,
    0b0000111110000,
    0b0000111110000,
    0b0000110000000,
    0b0000110000000,
    0b0000111110000,
    0b0000111110000,
    0b0000000000000
  },
  {
    0b0000000000000,
    0b0000110110000,
    0b0000110110000,
    0b0000110110000,
    0b0000110110000,
    0b0000111110000,
    0b0000111110000,
    0b0000000110000,
    0b0000000110000,
    0b0000000110000,
    0b0000000110000,
    0b0000000110000,
    0b0000000000000
  },
  {
    0b0000000000000,
    0b0000111110000,
    0b0000111110000,
    0b0000110110000,
    0b0000110110000,
    0b0000111110000,
    0b0000110110000,
    0b0000110110000,
    0b0000110110000,
    0b0000111110000,
    0b0000111110000,
    0b0000000000000
  },
  {
    0b0000000000000,
    0b0111101111100,
    0b0111101111100,
    0b0001101100000,
    0b0001101100000,
    0b0001101111100,
    0b0001101111100,
    0b0001101101100,
    0b0001101101100,
    0b0001101101100,
    0b0001101111100,
    0b0001101111100,
    0b0000000000000
  },
  {
    0b0000000000000,
    0b0011110111110,
    0b0011110111110,
    0b0000110000110,
    0b0000110000110,
    0b0000110000110,
    0b0011110111110,
    0b0011110111110,
    0b0000110110000,
    0b0000110110000,
    0b0011110111110,
    0b0011110111110,
    0b0000000000000
  },
  {
    0b0000000000000,
    0b0111110110110,
    0b0111110110110,
    0b0110000110110,
    0b0110000110110,
    0b0111110111110,
    0b0111110111110,
    0b0110110000110,
    0b0110110000110,
    0b0110110000110,
    0b0111110000110,
    0b0111110000110,
    0b0000000000000
  },
  {
    0b0000000000000,
    0b0000000011000,
    0b0000000001000,
    0b0000000001000,
    0b0000000001000,
    0b0000000001000,
    0b0000000000000,
    0b0001110111000,
    0b0000010101000,
    0b0001110111000,
    0b0001000101000,
    0b0001110111000,
    0b0000000000000
  },
  {
    0b0000000000000,
    0b0000000111000,
    0b0000000001000,
    0b0000000111000,
    0b0000000100000,
    0b0000000111000,
    0b0000000000000,
    0b0001110111000,
    0b0001000100000,
    0b0001110111000,
    0b0000010101000,
    0b0001110111000,
    0b0000000000000
  },
  {
    0b0000000000000,
    0b0000000111000,
    0b0000000100000,
    0b0000000111000,
    0b0000000001000,
    0b0000000111000,
    0b0000000000000,
    0b0000110111000,
    0b0000010001000,
    0b0000010111000,
    0b0000010100000,
    0b0000010111000,
    0b0000000000000
  },
  {
    0b0000000000000,
    0b0000110111000,
    0b0000010101000,
    0b0000010101000,
    0b0000010101000,
    0b0000010111000,
    0b0000000000000,
    0b0001110101000,
    0b0000010101000,
    0b0001110111000,
    0b0001000001000,
    0b0001110001000,
    0b0000000000000
  },
  {
    0b0000000000000,
    0b0001110111000,
    0b0000010101000,
    0b0001110101000,
    0b0001000101000,
    0b0001110111000,
    0b0000000000000,
    0b0001010111000,
    0b0001010101000,
    0b0001110111000,
    0b0000010101000,
    0b0000010111000,
    0b0000000000000
  },
  {
    0b0000000000000,
    0b0001010111000,
    0b0001010101000,
    0b0001110101000,
    0b0000010101000,
    0b0000010111000,
    0b0000000000000,
    0b0001110111000,
    0b0001010100000,
    0b0001110111000,
    0b0000010101000,
    0b0001110111000,
    0b0000000000000
  },
  {
    0b0000000000000,
    0b0001110011000,
    0b0001010001000,
    0b0001110001000,
    0b0001010001000,
    0b0001110001000,
    0b0000000000000,
    0b0001110111000,
    0b0001010001000,
    0b0001110111000,
    0b0000010100000,
    0b0001110111000,
    0b0000000000000
  }
};

void render_intro(badge_framebuffer * fb) {
  (void) fb;
  badge_event_t ev;

  badge_framebuffer_clear(fb);
  badge_framebuffer_render_text(fb, 2 * BADGE_FONT_WIDTH, BADGE_DISPLAY_HEIGHT / 2 - BADGE_FONT_HEIGHT,
                                "A oder B zum");
  badge_framebuffer_render_text(fb, BADGE_FONT_WIDTH / 2, BADGE_DISPLAY_HEIGHT / 2,
                                "Beenden drücken");

  badge_framebuffer_render_text(fb, 3 * BADGE_FONT_WIDTH, BADGE_DISPLAY_HEIGHT / 2 + 2 * BADGE_FONT_HEIGHT,
                                "by lykaner");

  badge_framebuffer_flush((badge_framebuffer const *) fb);

  for(uint8_t i = 0; i < 100;) {
    ev = badge_event_wait();
    if(badge_event_type(ev) == BADGE_EVENT_GAME_TICK)
      i++;
  }
}

void invert_framebuffer(badge_framebuffer * fb) {
  for(size_t i = 0; i < sizeof(fb->data); ++i) {
    fb->data[0][i] ^= 0xff;
  }

  badge_framebuffer_flush((badge_framebuffer const *) fb);
}

void render_lost(badge_framebuffer * fb) {
  badge_event_t ev;

  badge_framebuffer_clear(fb);
  badge_framebuffer_render_text(fb, 10, BADGE_DISPLAY_HEIGHT / 2 - BADGE_FONT_HEIGHT,
                                "YOU LOST");
  badge_framebuffer_flush((badge_framebuffer const *) fb);

  for(uint8_t i = 0; i < 100;) {
    ev = badge_event_wait();
    if(badge_event_type(ev) == BADGE_EVENT_GAME_TICK)
      i++;
    if(i % 25 == 0)
      invert_framebuffer(fb);
  }
}

void show_frame(badge_framebuffer * fb) {
  for(uint8_t y = 10; y <= 10 + 14 * 4; y += 14) {
    for(uint8_t x = 20; x <= 20 + 55; x++)
      badge_framebuffer_pixel_on(fb, x, y);
  }

  for(uint8_t x = 20; x <= 20 + 4 * 14; x += 14) {
    for(uint8_t y = 10; y <= 10 + 14 * 4; y++)
      badge_framebuffer_pixel_on(fb, x, y);
  }

  badge_framebuffer_flush((badge_framebuffer const *) fb);
}

void render_score(badge_framebuffer * fb, uint32_t score) {
  uint8_t digit;
  uint32_t pow10;

  pow10 = 1;
  for(uint8_t i = 0; i < 9; i++) {
    score /= pow10;
    digit = score % 10;
    score *= pow10;
    pow10 *= 10;
    badge_framebuffer_render_number(fb, 70 - (i * BADGE_FONT_WIDTH), 1, digit);
  }

  badge_framebuffer_flush((badge_framebuffer const *) fb);
}

void show_grid(badge_framebuffer * fb, uint8_t grid[GRID_SIZE][GRID_SIZE]) {
  for(uint8_t x_grid = 0; x_grid < 4; x_grid++) {
    for(uint8_t y_grid = 0; y_grid < 4; y_grid++) {
        render_number(fb, grid[x_grid][y_grid], x_grid, y_grid);
    }
  }

  badge_framebuffer_flush((badge_framebuffer const *) fb);
}

void render_number(badge_framebuffer * fb, uint8_t number, uint8_t x_grid, uint8_t y_grid) {
  for(uint8_t y = 11 + y_grid * 15; y < 11 + y_grid * 15 + 13; y++) {
    for(uint8_t x = 21 + x_grid * 15; x < 21 + x_grid * 15 + 13; x++) {
      if((number_glyphs[number][(y - (11 + y_grid * 15))] >> (21 + x_grid * 15 + 12 - x)) & 1)
         badge_framebuffer_pixel_on(fb, x - x_grid, y - y_grid);
      else
         badge_framebuffer_pixel_off(fb, x - x_grid, y - y_grid);
    }
  }
};
