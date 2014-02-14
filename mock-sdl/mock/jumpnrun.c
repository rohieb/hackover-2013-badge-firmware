#include "jumpnrun.h"
#include "display.h"
#include "main_loop.h"

#include <jumpnrun/game_state.h>
#include <jumpnrun/jumpnrun.h>
#include <ui/display.h>

#include <stdlib.h>

//static void mock_jumpnrun_tick(void);

static jumpnrun_level      mock_jnr_level;
static jumpnrun_game_state mock_jnr_state;

static void mock_jumpnrun_level_free(jumpnrun_level *lv) {
  free(lv->tiles);
  free(lv->items);
  free(lv->enemies);

  memset(lv, 0, sizeof(*lv));
}

static int mock_jumpnrun_start_level_fd(FILE *fd) {
  mock_jumpnrun_level_free(&mock_jnr_level);

  int err = jumpnrun_load_level_header_from_file(&mock_jnr_level, fd);

  if(err != 0) {
    return err;
  }

  mock_jnr_level.tiles   = malloc(sizeof(*mock_jnr_level.tiles  ) * mock_jnr_level.header. tile_count);
  mock_jnr_level.items   = malloc(sizeof(*mock_jnr_level.items  ) * mock_jnr_level.header. item_count);
  mock_jnr_level.enemies = malloc(sizeof(*mock_jnr_level.enemies) * mock_jnr_level.header.enemy_count);

  if(mock_jnr_level.tiles   == NULL ||
     mock_jnr_level.items   == NULL ||
     mock_jnr_level.enemies == NULL)
  {
    mock_jumpnrun_level_free(&mock_jnr_level);
    return JUMPNRUN_ERROR;
  }

  err = jumpnrun_load_level_from_file(&mock_jnr_level, fd);
  if(err != 0) {
    jumpnrun_game_state_init   (&mock_jnr_state, &mock_jnr_level);
    jumpnrun_game_state_respawn(&mock_jnr_state, &mock_jnr_level);

//    mock_set_main_loop_tick(mock_jumpnrun_tick);
  }

  return err;
}

int mock_jumpnrun_start_level(char const *fname) {
  FILE *fd = fopen(fname, "rb");

  if(fd == NULL) {
    return JUMPNRUN_ERROR;
  }

  int err = mock_jumpnrun_start_level_fd(fd);
  fclose(fd);
  return err;
}
