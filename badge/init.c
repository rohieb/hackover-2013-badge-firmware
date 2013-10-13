#include "init.h"
#include "pinconfig.h"
#include "util/util.h"
#include "ui/display.h"

#include <core/wdt/wdt.h>
#include <core/gpio/gpio.h>

static void badge_init_backlight(void) {
  HOB_SET_PIN_FUNC(HOB_LCD_BACKLIGHT, CLKOUT);

  wdtInit(false);
  SCB_CLKOUTCLKSEL = SCB_MAINCLKSEL_SOURCE_WDTOSC;
  SCB_CLKOUTCLKUEN = SCB_CLKOUTCLKUEN_DISABLE;
  SCB_CLKOUTCLKUEN = SCB_CLKOUTCLKUEN_UPDATE;
  SCB_CLKOUTCLKDIV = 30;
}

void badge_init(void) {
#define INPUT_PIN_CONFIG(spec, mode) do {				\
    HOB_SET_PIN_FUNC(spec, GPIO);					\
    gpioSetDir(HOB_PORT(spec), HOB_PIN(spec), gpioDirection_Input);	\
    gpioSetPullup(&HOB_IOCON(spec), (mode));				\
  } while(0)

#ifdef HOB_REV2
  INPUT_PIN_CONFIG(HOB_BTN_UP    , gpioPullupMode_PullUp);
  INPUT_PIN_CONFIG(HOB_BTN_RIGHT , gpioPullupMode_PullUp);
  INPUT_PIN_CONFIG(HOB_BTN_DOWN  , gpioPullupMode_PullUp);
  INPUT_PIN_CONFIG(HOB_BTN_LEFT  , gpioPullupMode_PullUp);
  INPUT_PIN_CONFIG(HOB_BTN_A     , gpioPullupMode_PullUp);
  INPUT_PIN_CONFIG(HOB_BTN_B     , gpioPullupMode_PullUp);
#else
  INPUT_PIN_CONFIG(HOB_BTN_UP    , gpioPullupMode_PullDown);
  INPUT_PIN_CONFIG(HOB_BTN_RIGHT , gpioPullupMode_PullDown);
  INPUT_PIN_CONFIG(HOB_BTN_DOWN  , gpioPullupMode_PullDown);
  INPUT_PIN_CONFIG(HOB_BTN_LEFT  , gpioPullupMode_PullDown);
  INPUT_PIN_CONFIG(HOB_BTN_CENTER, gpioPullupMode_PullDown);
  INPUT_PIN_CONFIG(HOB_BTN_A     , gpioPullupMode_PullDown);
  INPUT_PIN_CONFIG(HOB_BTN_B     , gpioPullupMode_PullDown);
#endif

#undef INPUT_PIN_CONFIG

#define OUTPUT_PIN_CONFIG(spec, value) do {				\
    HOB_SET_PIN_FUNC(spec, GPIO);					\
    gpioSetDir(HOB_PORT(spec), HOB_PIN(spec), gpioDirection_Output);	\
    gpioSetValue(HOB_PORT(spec), HOB_PIN(spec), (value));		\
  } while(0)

  OUTPUT_PIN_CONFIG(HOB_USB_CONNECT , 1);
  OUTPUT_PIN_CONFIG(HOB_LCD_CS      , 1);
  OUTPUT_PIN_CONFIG(HOB_LCD_RST     , 1);
  OUTPUT_PIN_CONFIG(HOB_DATAFLASH_CS, 1);

#ifdef HOB_REV2
  OUTPUT_PIN_CONFIG(HOB_LED_LEFT    , 0);
  OUTPUT_PIN_CONFIG(HOB_LED_RIGHT   , 0);
#endif

#undef OUTPUT_PIN_CONFIG

  badge_init_backlight();
  badge_display_init();
}
