#ifndef INCLUDED_BADGE_UTIL_RANDOM_H
#define INCLUDED_BADGE_UTIL_RANDOM_H

#include "fixed_point.h"

#include <stdint.h>

typedef struct badge_rng {
  uint32_t state;
} badge_rng;

static inline badge_rng badge_rng_init(uint32_t seed) { return (badge_rng) { seed }; }

uint8_t     badge_rng_u8         (badge_rng *rng);
uint32_t    badge_rng_u32        (badge_rng *rng);
fixed_point badge_rng_fixed_point(badge_rng *rng);

#endif
