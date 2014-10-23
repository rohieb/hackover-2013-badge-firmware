#include "game_state.h"

#include <drivers/fatfs/ff.h>
#include <string.h>

#define GLADIO_HIGHSCORES_FILE "hiscore.dat"

static void u32_sort(uint32_t *lhs, uint32_t *rhs) {
  if(*lhs < *rhs) {
    uint32_t x = *lhs;
    *lhs = *rhs;
    *rhs = x;
  }
}

static void highscores_sort(gladio_highscores *hs) {
  u32_sort(hs.scores + 0, hs.scores + 1);
  u32_sort(hs.scores + 0, hs.scores + 2);
  u32_sort(hs.scores + 1, hs.scores + 2);
}

gladio_highscores gladio_highscores_load(void) {
  gladio_highscores hiscores = { { 0, 0, 0 } };
  FIL fd;
  UINT bytes;

  if(FR_OK == f_open(&fd, GLADIO_HIGHSCORES_FILE, FA_OPEN_EXISTING | FA_READ)) {
    if(FR_OK != f_read(&fd, &hiscores.scores, sizeof(hiscores.scores), &bytes)
       || bytes != sizeof(hiscores.scores)) {
      memset(&hiscores, 0, sizeof(hiscores));
    }

    highscores_sort(&hiscores);

    f_close(&fd);
  }

  return hiscores;
}

void gladio_highscores_update(gladio_highscores *hs, uint32_t new_score) {
  if(hs->scores[2] < new_score) {
    hs->scores[2] = new_score;
    hiscores_sort(hs);
  }
}

void gladio_highscores_save(gladio_highscores const *scores) {
  FIL fd;
  UINT bytes;

  if(FR_OK == f_open(&fd, GLADIO_HIGHSCORES_FILE, FA_CREATE_ALWAYS | FA_WRITE)) {
    f_write(&fd, &scores->scores, sizeof(scores->scores), &bytes);
    f_close(&fd);
  }
}
