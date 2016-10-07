#ifndef INCLUDED_BADGE2013_MOCKUP_DISPLAY_H
#define INCLUDED_BADGE2013_MOCKUP_DISPLAY_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

  enum {
    BADGE_DISPLAY_WIDTH         = 96,
    BADGE_DISPLAY_HEIGHT        = 68,

    BADGE_DISPLAY_STRIPE_HEIGHT = 8,
    BADGE_DISPLAY_STRIPE_COUNT  = (BADGE_DISPLAY_HEIGHT - 1) / BADGE_DISPLAY_STRIPE_HEIGHT + 1
  };

  typedef uint8_t badge_display_stripe[BADGE_DISPLAY_WIDTH];
  typedef struct {
    badge_display_stripe data[BADGE_DISPLAY_STRIPE_COUNT];
  } badge_framebuffer;

  void badge_display_init(void);
  void badge_framebuffer_flush(badge_framebuffer const *fb);

  static inline void badge_framebuffer_clear(badge_framebuffer *fb) {
    memset(fb, 0, sizeof(*fb));
  }

  static inline uint8_t badge_framebuffer_pixel(badge_framebuffer const *fb,
                                                uint8_t x,
                                                uint8_t y) {
    return fb->data[y / BADGE_DISPLAY_STRIPE_HEIGHT][x] >> (y % BADGE_DISPLAY_STRIPE_HEIGHT) & 1;
  }

  static inline void badge_framebuffer_pixel_on(badge_framebuffer *fb,
                                                uint8_t x,
                                                uint8_t y) {
    fb->data[y / BADGE_DISPLAY_STRIPE_HEIGHT][x] |= 1 << (y % BADGE_DISPLAY_STRIPE_HEIGHT);
  }

  static inline void badge_framebuffer_pixel_off(badge_framebuffer *fb,
                                                 uint8_t x,
                                                 uint8_t y) {
    fb->data[y / BADGE_DISPLAY_STRIPE_HEIGHT][x] &= ~(1 << (y % BADGE_DISPLAY_STRIPE_HEIGHT));
  }

  static inline void badge_framebuffer_pixel_flip(badge_framebuffer *fb,
                                                  uint8_t x,
                                                  uint8_t y) {
    fb->data[y / BADGE_DISPLAY_STRIPE_HEIGHT][x] ^= 1 << (y % BADGE_DISPLAY_STRIPE_HEIGHT);
  }

  static inline void badge_framebuffer_pixel_set(badge_framebuffer *fb,
                                                 uint8_t x,
                                                 uint8_t y,
                                                 uint8_t val) {
    if(val) {
      badge_framebuffer_pixel_on (fb, x, y);
    } else {
      badge_framebuffer_pixel_off(fb, x, y);
    }
  }

#ifdef __cplusplus
}
#endif

#endif
