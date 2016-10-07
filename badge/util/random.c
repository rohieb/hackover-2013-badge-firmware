#include "random.h"

static void badge_rng_advance(badge_rng *rng) {
  rng->state ^= rng->state <<  1;
  rng->state ^= rng->state >>  3;
  rng->state ^= rng->state << 10;
}

uint32_t badge_rng_u32(badge_rng *rng) {
  badge_rng_advance(rng);
  return rng->state;
}

uint8_t badge_rng_u8 (badge_rng *rng) {
  return (uint8_t) badge_rng_u32(rng);
}

fixed_point badge_rng_fixed_point(badge_rng *rng) {
  return (fixed_point) { (int32_t) (badge_rng_u32(rng) & (((uint32_t) -1) >> 1)) };
}
