#include "sprites.h"

static badge_sprite const common_sprites[] = {
  { 5, 7, (uint8_t const *) "\x04\xc3\xdf\x40" },
  { 5, 7, (uint8_t const *) "\x10\xd8\x1f\x06\x01" }
};

badge_sprite const *common_sprite(int id) {
  return &common_sprites[id];
}
