#include "sprite.h"

//#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>

typedef struct  {
  uint8_t const *data_ptr;
  uint8_t        left_in_byte;
} sprite_column_iterator;

static inline  int8_t i8min( int8_t x,  int8_t y) { return x < y ? x : y; }
static inline  int8_t i8max( int8_t x,  int8_t y) { return x < y ? y : x; }
static inline uint8_t u8min(uint8_t x, uint8_t y) { return x < y ? x : y; }
//static inline uint8_t u8max(uint8_t x, uint8_t y) { return x < y ? y : x; }

static inline sprite_column_iterator sprite_column_iterator_init(badge_sprite const *sprite, uint8_t xskip) {
  sprite_column_iterator result = { sprite->data, CHAR_BIT };

  if(xskip != 0) {
    int skip_bits = (xskip * sprite->height);

    result.data_ptr      += skip_bits / CHAR_BIT;
    result.left_in_byte   = CHAR_BIT - skip_bits % 8;
  }

  return result;
}

static inline uint32_t sprite_column_iterator_pop(sprite_column_iterator *iter, uint8_t height) {
  uint32_t column = 0;

  if(iter->left_in_byte > height) {
    column = *iter->data_ptr >> (CHAR_BIT - iter->left_in_byte) & ((1 << height) - 1);
    iter->left_in_byte -= height;
  } else {
    column = *iter->data_ptr >> (CHAR_BIT - iter->left_in_byte);

    uint8_t shlen = iter->left_in_byte;
    uint8_t n;

    for(n = height - shlen; n >= CHAR_BIT; n -= CHAR_BIT) {
      column |= *++iter->data_ptr << shlen;
      shlen += CHAR_BIT;
    }

    column |= (*++iter->data_ptr & ((1 << n) - 1)) << shlen;
    iter->left_in_byte = CHAR_BIT - n;
  }

  return column;
}

void badge_framebuffer_blt(badge_framebuffer *fb,
                           int8_t x,
                           int8_t y,
                           badge_sprite const *sprite,
                           unsigned flags)
{
  if(y >= BADGE_DISPLAY_HEIGHT ||
     x >= BADGE_DISPLAY_WIDTH  ||
     y <= -sprite->height ||
     x <= -sprite->width) {
    // Nichts zu tun.
    return;
  }

  int8_t  y_major = y / BADGE_DISPLAY_STRIPE_HEIGHT - (y % BADGE_DISPLAY_STRIPE_HEIGHT < 0);
  uint8_t y_minor = (uint8_t) (y - y_major * BADGE_DISPLAY_STRIPE_HEIGHT);

  int8_t xmove;
  int8_t xcursor;
  int8_t xend;
  uint8_t xskip;

  if(flags & BADGE_BLT_MIRRORED) {
    xmove   = -1;
    xcursor =  i8min(x + sprite->width, BADGE_DISPLAY_WIDTH) - 1;
    xend    =  i8max(x, 0) - 1;
    xskip   =  x + sprite->width - xcursor - 1;
  } else {
    xmove   =  1;
    xcursor =  i8max(x, 0);
    xend    =  i8min(x + sprite->width, BADGE_DISPLAY_WIDTH);
    xskip   =  xcursor - x;
  }

  sprite_column_iterator col_iter = sprite_column_iterator_init(sprite, xskip);

  while(xcursor != xend) {
    uint32_t column = sprite_column_iterator_pop(&col_iter, sprite->height);

/*
    assert(xcursor >= 0);
    assert(xcursor < BADGE_DISPLAY_WIDTH);
*/

    if(y_major >= 0) {
      fb->data[y_major][xcursor] |= (uint8_t) column << y_minor;
    }

    uint8_t stripe_max = u8min((sprite->height + y_minor - 1) / BADGE_DISPLAY_STRIPE_HEIGHT,
                               BADGE_DISPLAY_STRIPE_COUNT - y_major - 1);

    for(uint8_t stripe_ptr = i8max(1, -y_major); stripe_ptr <= stripe_max; ++stripe_ptr) {
/*
      assert(y_major + stripe_ptr >= 0);
      assert(y_major + stripe_ptr < BADGE_DISPLAY_STRIPE_COUNT);
*/

      fb->data[y_major + stripe_ptr][xcursor] |= (uint8_t) (column >> (stripe_ptr * BADGE_DISPLAY_STRIPE_HEIGHT - y_minor));
    }

    xcursor += xmove;
  }
}
