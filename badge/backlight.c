#include "backlight.h"
#include "pinconfig.h"
#include <lpc134x.h>

#include <core/wdt/wdt.h>

void badge_backlight_init(void) {
  HOB_SET_PIN_FUNC(HOB_LCD_BACKLIGHT, CLKOUT);

  wdtInit(false);
  SCB_CLKOUTCLKSEL = SCB_MAINCLKSEL_SOURCE_WDTOSC;
  SCB_CLKOUTCLKUEN = SCB_CLKOUTCLKUEN_DISABLE;
  SCB_CLKOUTCLKUEN = SCB_CLKOUTCLKUEN_UPDATE;

  badge_backlight_enable();
}

void badge_backlight_enable(void) {
  SCB_CLKOUTCLKDIV = 75;
}

void badge_backlight_disable(void) {
  SCB_CLKOUTCLKDIV =  0;
}
