/**************************************************************************/
/*!
    @file     main.c
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2011, microBuilder SARL
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "projectconfig.h"
#include "sysinit.h"

#include "core/adc/adc.h"
#include "core/cpu/cpu.h"
#include "core/pmu/pmu.h"
#include "core/wdt/wdt.h"
#include "core/gpio/gpio.h"
#include "core/ssp/ssp.h"
#include "core/systick/systick.h"
#include "core/usbhid-rom/usbmsc.h"

#ifdef CFG_INTERFACE
  #include "core/cmd/cmd.h"
#endif

#include "fahrplan.h"
#include "init.h"
#include "ui/browser.h"
#include "ui/display.h"
#include "ui/event.h"
#include "ui/font.h"
#include "ui/menu.h"
#include "ui/sprite.h"
#include "ui/vanity.h"
#include "util/util.h"
#include "jumpnrun/jumpnrun.h"

#include "dataflash/at45db041d.h"

#include "drivers/fatfs/ff.h"

#include "badge/pinconfig.h"
#include <funk/nrf24l01p.h>

#ifdef R0KET

#include "r0ketports.h"
void rbBacklightInit(void) {
  /* Enable the clock for CT16B1 */
  SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_CT16B1);

  /* Configure PIO1.9 as Timer1_16 MAT0 Output */
  IOCON_PIO1_9 &= ~IOCON_PIO1_9_FUNC_MASK;
  IOCON_PIO1_9 |=  IOCON_PIO1_9_FUNC_CT16B1_MAT0;

  /* Set default duty cycle (MR1) */
  TMR_TMR16B1MR0 = 0; //(0xFFFF * (100 - brightness)) / 100;

  /* External Match Register Settings for PWM */
  TMR_TMR16B1EMR = TMR_TMR16B1EMR_EMC0_TOGGLE | TMR_TMR16B1EMR_EM0;

  /* enable Timer1 */
  TMR_TMR16B1TCR = TMR_TMR16B1TCR_COUNTERENABLE_ENABLED;

  /* Enable PWM0 */
  TMR_TMR16B1PWMC = TMR_TMR16B1PWMC_PWM0_ENABLED;

  // Enable Step-UP
  gpioSetDir(RB_PWR_LCDBL, gpioDirection_Output);
  gpioSetValue(RB_PWR_LCDBL, 0);
}

void rbInit() {
  RB_HB0_IO &= ~IOCON_SWDIO_PIO1_3_FUNC_MASK;
  RB_HB0_IO |=  IOCON_SWDIO_PIO1_3_FUNC_GPIO;
  RB_HB1_IO &= ~IOCON_JTAG_TCK_PIO0_10_FUNC_MASK;
  RB_HB1_IO |=  IOCON_JTAG_TCK_PIO0_10_FUNC_GPIO;

  struct {
    int port;
    int pin;
    uint32_t volatile *reg;
    gpioPullupMode_t mode;
  } const input_pins[] = {
    { RB_BTN0    , &RB_BTN0_IO    , gpioPullupMode_PullUp },
    { RB_BTN1    , &RB_BTN1_IO    , gpioPullupMode_PullUp },
    { RB_BTN2    , &RB_BTN2_IO    , gpioPullupMode_PullUp },
    { RB_BTN3    , &RB_BTN3_IO    , gpioPullupMode_PullUp },
    { RB_BTN4    , &RB_BTN4_IO    , gpioPullupMode_PullUp },
    { RB_HB0     , &RB_HB0_IO     , gpioPullupMode_PullUp },
    { RB_HB1     , &RB_HB1_IO     , gpioPullupMode_PullUp },
    { RB_PWR_CHRG, &RB_PWR_CHRG_IO, gpioPullupMode_PullUp }
  };

  for(int i = 0; i < ARRAY_SIZE(input_pins); ++i) {
    gpioSetDir(input_pins[i].port, input_pins[i].pin, gpioDirection_Input);
    gpioSetPullup(input_pins[i].reg, input_pins[i].mode);
  }

  // LED3 zur Bestimmung der Umgebungshelligkeit.
  gpioSetDir(RB_LED3, gpioDirection_Input);
  RB_LED3_IO = (RB_LED3_IO & IOCON_PIO1_11_FUNC_MASK) | IOCON_PIO1_11_FUNC_AD7;

  // prepare LEDs
  IOCON_JTAG_TDI_PIO0_11 &= ~IOCON_JTAG_TDI_PIO0_11_FUNC_MASK;
  IOCON_JTAG_TDI_PIO0_11 |=  IOCON_JTAG_TDI_PIO0_11_FUNC_GPIO;

  struct {
    int port;
    int pin;
    int value;
  } const output_pins[] = {
    { RB_PWR_GOOD , 0 },
    { USB_CONNECT , 1 },
    { RB_LCD_CS   , 1 },
    { RB_SPI_CS_DF, 1 },
    { RB_SPI_SS2 , 1 },
    { RB_SPI_SS3 , 1 },
    { RB_SPI_SS4 , 1 },
    { RB_SPI_SS5 , 1 },
    { RB_LED0    , 0 },
    { RB_LED1    , 0 },
    { RB_LED2    , 0 },
    { RB_LCD_BL  , 0 },
    { RB_HB2     , 1 },
    { RB_HB3     , 1 },
    { RB_HB4     , 1 },
    { RB_HB5     , 1 }
  };

  for(int i = 0; i < ARRAY_SIZE(output_pins); ++i) {
    gpioSetDir  (output_pins[i].port, output_pins[i].pin, gpioDirection_Output);
    gpioSetValue(output_pins[i].port, output_pins[i].pin, output_pins[i].value);
  }

  // Set P0.0 to GPIO
  RB_PWR_LCDBL_IO &= ~RB_PWR_LCDBL_IO_FUNC_MASK;
  RB_PWR_LCDBL_IO |=  RB_PWR_LCDBL_IO_FUNC_GPIO;

  gpioSetDir   ( RB_PWR_LCDBL   , gpioDirection_Input);
  gpioSetPullup(&RB_PWR_LCDBL_IO, gpioPullupMode_Inactive);

  rbBacklightInit();
  badge_display_init();
}

#endif

static void usbmode(void) {
  badge_event_stop();

  badge_framebuffer fb = { { { 0 } } };
  badge_framebuffer_render_text(&fb, 22, 20, "USB-Modus");
//  badge_framebuffer_render_text(&fb, 10, 35, "Reset to exit");
  badge_framebuffer_render_text(&fb, 16, 35, "Zurück mit");
  badge_framebuffer_render_text(&fb, 32, 45, "Reset");
/*
    badge_framebuffer_render_number(&fb, 23, 50, sizeof(jumpnrun_tile));
    badge_framebuffer_render_number(&fb, 33, 50, sizeof(jumpnrun_item));
    badge_framebuffer_render_number(&fb, 48, 50, sizeof(jumpnrun_enemy));
*/
  badge_framebuffer_flush(&fb);

  usbMSCInit();

  for(;;) {
//    pmuSleep();
  }
}

uint8_t main_menu_show(uint8_t selected) {
  // first_visible = 0, weil das Menü so kurz ist. Sollte es
  // größer werden: Parameter aus main_menu empfangen und merken.
  uint8_t first_visible = 0;
  char const *const menu[] = {
    "Titelbild",
    "Super Hackio",
    "Fahrplan",
    "USB-Modus"
  };

  f_chdir("/");
  return (uint8_t) badge_menu(menu, ARRAY_SIZE(menu), &first_visible, selected);
}

void main_menu(void) {
  uint8_t selected = 0;

  for(;;) {
    selected = main_menu_show(selected);
    switch(selected) {
    case 0: badge_vanity_show(); break;
    case 1: jumpnrun_play    (); break;
    case 2: badge_fahrplan   (); break;
    case 3: usbmode          (); break;
    }
  }
}

int main(void)
{
  cpuInit();
  systickInit(CFG_SYSTICK_DELAY_IN_MS);

  pmuInit();
  // adcInit();
#ifdef R0KET
  rbInit();
#else
  badge_init();
#endif

  FATFS fs;
  f_mount(0, &fs);

#ifdef USBONLY
  if(badge_input_raw() & BADGE_EVENT_KEY_LEFT) {
    gpioSetValue(HOB_PORT(HOB_LED_LEFT), HOB_PIN(HOB_LED_LEFT) , 1);
  }
  if(badge_input_raw() & BADGE_EVENT_KEY_RIGHT) {
    gpioSetValue(HOB_PORT(HOB_LED_RIGHT), HOB_PIN(HOB_LED_RIGHT) , 1);
  }

  for(;;) usbmode();
#else
  if(badge_input_raw() & BADGE_EVENT_KEY_DOWN) {
    usbmode();
  }

  badge_event_start();
  badge_vanity_show();
  main_menu();
#endif

  return 0;
}
