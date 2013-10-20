#include "shots.h"
#include "jumpnrun.h"
#include "render.h"

void jumpnrun_shot_process(jumpnrun_shot *shot) {
  if(jumpnrun_shot_spawned(shot)) {
    rectangle_move_rel(&shot->current_box, shot->inertia);
    jumpnrun_apply_gravity(&shot->inertia);

    if(fixed_point_gt(rectangle_top(&shot->current_box), FIXED_INT(BADGE_DISPLAY_HEIGHT))) {
      jumpnrun_shot_despawn(shot);
    }

    ++shot->tick;
    if(shot->tick == JUMPNRUN_SHOT_FRAMES * JUMPNRUN_SHOT_TICKS_PER_FRAME) {
      shot->tick = 0;
    }
  }
}
