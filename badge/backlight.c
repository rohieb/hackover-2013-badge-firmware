#include "backlight.h"
#include "pinconfig.h"
#include <lpc134x.h>

#include "common/sprites.h"
#include "ui/display.h"
#include "ui/event.h"
#include "ui/font.h"
#include "ui/sprite.h"

#include <core/wdt/wdt.h>
#include <drivers/fatfs/ff.h>

#define BACKLIGHT_CONFIG_FILE "/config/blclkdiv.dat"

static uint8_t backlight_clkdiv_user = 75;

void badge_backlight_enable(void) {
  SCB_CLKOUTCLKDIV = backlight_clkdiv_user;
}

void badge_backlight_disable(void) {
  SCB_CLKOUTCLKDIV = 0;
}

void badge_backlight_load(void) {
  FIL fd;

  if(FR_OK == f_open(&fd, BACKLIGHT_CONFIG_FILE, FA_OPEN_EXISTING | FA_READ)) {
    UINT bytes;
    f_read(&fd, &backlight_clkdiv_user, sizeof(backlight_clkdiv_user), &bytes);
    f_close(&fd);
  }

  badge_backlight_enable();
}

void badge_backlight_save(void) {
  FIL fd;

  if(FR_OK == f_open(&fd, BACKLIGHT_CONFIG_FILE, FA_CREATE_ALWAYS | FA_WRITE)) {
    UINT bytes;
    f_write(&fd, &backlight_clkdiv_user, sizeof(backlight_clkdiv_user), &bytes);
    f_close(&fd);
  }
}

void badge_backlight_init(void) {
  HOB_SET_PIN_FUNC(HOB_LCD_BACKLIGHT, CLKOUT);

  wdtInit(false);
  SCB_CLKOUTCLKSEL = SCB_MAINCLKSEL_SOURCE_WDTOSC;
  SCB_CLKOUTCLKUEN = SCB_CLKOUTCLKUEN_DISABLE;
  SCB_CLKOUTCLKUEN = SCB_CLKOUTCLKUEN_UPDATE;

  badge_backlight_load();
}

void backlight_app(void) {
  unsigned scroll_ticks     = 25;
  int      scroll_direction = 0;

  for(;;) {
    if(scroll_ticks == 0) {
      if(scroll_direction == 1 && backlight_clkdiv_user < 255) {
        ++backlight_clkdiv_user;
        badge_backlight_enable();
      } else if(scroll_direction == -1 && backlight_clkdiv_user > 1) {
        --backlight_clkdiv_user;
        badge_backlight_enable();
      }

      badge_framebuffer fb = { { { 0 } } };
      badge_framebuffer_render_text  (&fb, 6,  10, "Frequenzteiler");
      badge_framebuffer_render_text  (&fb, 27, 18, "f\xfcr die");
      badge_framebuffer_render_text  (&fb, 21, 26, "Ladepumpe");

      badge_framebuffer_blt          (&fb, 45, 36, common_sprite(BADGE_COMMON_SPRITE_ARROW_UP  ), 0);
      badge_framebuffer_render_number(&fb, 39, 44, backlight_clkdiv_user);
      badge_framebuffer_blt          (&fb, 45, 52, common_sprite(BADGE_COMMON_SPRITE_ARROW_DOWN), 0);
      badge_framebuffer_flush        (&fb);

      scroll_ticks = (badge_event_current_input_state() & BADGE_EVENT_KEY_BTN_B) ? 10 : 25;
    }

    badge_event_t ev = badge_event_wait();

    switch(badge_event_type(ev)) {
    case BADGE_EVENT_USER_INPUT:
      {
        uint8_t old_state = badge_event_old_input_state(ev);
        uint8_t new_state = badge_event_new_input_state(ev);
        uint8_t new_buttons = new_state & (old_state ^ new_state);

        if((new_buttons & BADGE_EVENT_KEY_BTN_A)) {
          badge_backlight_save();
          return;
        } else if((new_buttons & BADGE_EVENT_KEY_UP  )) {
          scroll_direction =  1;
        } else if((new_buttons & BADGE_EVENT_KEY_DOWN)) {
          scroll_direction = -1;
        } else if (old_state != (new_state ^ BADGE_EVENT_KEY_BTN_B)) {
          scroll_direction = 0;
        }

        scroll_ticks = 0;
        break;
      }

    case BADGE_EVENT_GAME_TICK:
      {
        --scroll_ticks;
      }
    }
  }
}
