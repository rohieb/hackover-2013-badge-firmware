#ifndef INCLUDED_HACKOVER_2013_BADGE_PINCONFIG_H
#define INCLUDED_HACKOVER_2013_BADGE_PINCONFIG_H

#include <lpc134x.h>

#define HOB_BTN_UP        (3,  2)
#define HOB_BTN_RIGHT     (1,  5)
#define HOB_BTN_DOWN      (1,  6)
#define HOB_BTN_LEFT      (3,  3)
#define HOB_BTN_CENTER    (1,  7)
#define HOB_BTN_A         (1, 11)
#define HOB_BTN_B         (1,  4)

#define HOB_LCD_CS        (1,  0)
#define HOB_LCD_RST       (1,  1)
#define HOB_LCD_BACKLIGHT (0,  1)

#define HOB_DATAFLASH_CS  (0,  7)

#define HOB_RADIO_CE      (2, 10)
#define HOB_RADIO_CS      (2,  9)
#define HOB_RADIO_IRQ     (2,  2)

#define HOB_USB_CONNECT   (0,  6)

#define HOB_PIO0_0 nRESET_PIO0_0
#define HOB_PIO0_1 PIO0_1
#define HOB_PIO0_2 PIO0_2
#define HOB_PIO0_3 PIO0_3
#define HOB_PIO0_4 PIO0_4
#define HOB_PIO0_5 PIO0_5
#define HOB_PIO0_6 PIO0_6
#define HOB_PIO0_7 PIO0_7
#define HOB_PIO0_8 PIO0_8
#define HOB_PIO0_9 PIO0_9
#define HOB_PIO0_10 JTAG_TCK_PIO0_10
#define HOB_PIO0_11 JTAG_TDI_PIO0_11

#define HOB_PIO1_0 JTAG_TMS_PIO1_0
#define HOB_PIO1_1 JTAG_TDO_PIO1_1
#define HOB_PIO1_2 JTAG_nTRST_PIO1_2
#define HOB_PIO1_3 SWDIO_PIO1_3
#define HOB_PIO1_4 PIO1_4
#define HOB_PIO1_5 PIO1_5
#define HOB_PIO1_6 PIO1_6
#define HOB_PIO1_7 PIO1_7
#define HOB_PIO1_8 PIO1_8
#define HOB_PIO1_9 PIO1_9
#define HOB_PIO1_10 PIO1_10
#define HOB_PIO1_11 PIO1_11

#define HOB_PIO2_0 PIO2_0
#define HOB_PIO2_1 PIO2_1
#define HOB_PIO2_2 PIO2_2
#define HOB_PIO2_3 PIO2_3
#define HOB_PIO2_4 PIO2_4
#define HOB_PIO2_5 PIO2_5
#define HOB_PIO2_6 PIO2_6
#define HOB_PIO2_7 PIO2_7
#define HOB_PIO2_8 PIO2_8
#define HOB_PIO2_9 PIO2_9
#define HOB_PIO2_10 PIO2_10
#define HOB_PIO2_11 PIO2_11

#define HOB_PIO3_0 PIO3_0
#define HOB_PIO3_1 PIO3_1
#define HOB_PIO3_2 PIO3_2
#define HOB_PIO3_3 PIO3_3
#define HOB_PIO3_4 PIO3_4
#define HOB_PIO3_5 PIO3_5

#define HOB_EXPAND(foo) foo

#define HOB_PORT_I(port, pin) port
#define HOB_PIN_I(port, pin) pin
#define HOB_PORT(spec) HOB_EXPAND(HOB_PORT_I spec)
#define HOB_PIN(spec)  HOB_EXPAND(HOB_PIN_I spec)

#define HOB_BUILD_PIO_SUFFIX_I(port, pin) HOB_PIO ## port ## _ ## pin
#define HOB_BUILD_PIO_SUFFIX(port, pin) HOB_BUILD_PIO_SUFFIX_I(port, pin)
#define HOB_PIO_SUFFIX(spec) HOB_BUILD_PIO_SUFFIX(HOB_PORT(spec), HOB_PIN(spec))

#define HOB_BUILD_IOCON_NAME_I(pio) IOCON_ ## pio
#define HOB_BUILD_IOCON_NAME(pio) HOB_BUILD_IOCON_NAME_I(pio)
#define HOB_IOCON(spec) HOB_BUILD_IOCON_NAME(HOB_PIO_SUFFIX(spec))

#define HOB_BUILD_IOCON_MASK_I(pio, flag) IOCON_ ## pio ## _ ## flag
#define HOB_BUILD_IOCON_MASK(pio, flag) HOB_BUILD_IOCON_MASK_I(pio, flag)
#define HOB_IOCON_MASK(spec, flag) HOB_BUILD_IOCON_MASK(HOB_PIO_SUFFIX(spec), flag)

#define HOB_SET_PIN_FUNC(spec, func) do {				\
    HOB_IOCON(spec) =							\
      (HOB_IOCON(spec) & ~HOB_IOCON_MASK(spec, FUNC_MASK))		\
      | HOB_IOCON_MASK(spec, FUNC_ ## func);				\
  } while(0)

#endif
