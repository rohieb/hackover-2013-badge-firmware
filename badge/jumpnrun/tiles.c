#include "tiles.h"

jumpnrun_tile_type const jumpnrun_tile_type_data[JUMPNRUN_TILE_TYPE_COUNT] = {
  {                         0, { 5, 5, (uint8_t const *) "\x3f\xf7\xfe\x01" } },
  {                         0, { 5, 5, (uint8_t const *) "\x3f\xc6\xf8\x01" } },
  {                         0, { 5, 5, (uint8_t const *) "\x2f\x87\x10\x00" } },
  {                         0, { 5, 5, (uint8_t const *) "\x21\x84\xfc\x00" } },
  {                         0, { 5, 5, (uint8_t const *) "\xe0\x03\x00\x00" } },
  {                         0, { 5, 5, (uint8_t const *) "\x00\x80\x0f\x00" } },
  { JUMPNRUN_COLLISION_TOP   , { 5, 5, (uint8_t const *) "\x98\x4e\x8a\x01" } },
  { JUMPNRUN_COLLISION_RIGHT , { 5, 5, (uint8_t const *) "\x3f\x2a\x42\x00" } },
  { JUMPNRUN_COLLISION_BOTTOM, { 5, 5, (uint8_t const *) "\xa3\xe4\x32\x00" } },
  { JUMPNRUN_COLLISION_LEFT  , { 5, 5, (uint8_t const *) "\x84\xa8\xf8\x01" } }
};
