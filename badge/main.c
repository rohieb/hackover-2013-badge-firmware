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
#include "core/systick/systick.h"
#include "core/usbhid-rom/usbmsc.h"

#ifdef CFG_INTERFACE
  #include "core/cmd/cmd.h"
#endif

#include "ui/display.h"
#include "ui/sprite.h"
#include "ui/event.h"
#include "jumpnrun/jumpnrun.h"

#include "r0ketports.h"
#include "drivers/fatfs/ff.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof*(arr))

void backlightInit(void) {
#if HW_IS_PROTOTYPE


  // prototype uses WDT CLKOUT to drive the LCD backlight
  // init without a reset

  IOCON_PIO0_1 &= ~(IOCON_PIO0_1_FUNC_MASK);
  IOCON_PIO0_1 |=   IOCON_PIO0_1_FUNC_CLKOUT;

  wdtInit(false);

  // use the WDT clock as the default WDT frequency is best frequency
  // XXX this register value is missing in lpc1343.h
#define SCB_CLKOUTCLKSEL_SOURCE_WDTCLOCK          ((unsigned int) 0x00000002) // Use the WDT clock

  SCB_CLKOUTCLKSEL = SCB_CLKOUTCLKSEL_SOURCE_WDTCLOCK;

  // toggle from LOW to HIGH to update source
  SCB_CLKOUTCLKUEN = SCB_CLKOUTCLKUEN_DISABLE;
  SCB_CLKOUTCLKUEN = SCB_CLKOUTCLKUEN_UPDATE;

  // divide by 30, to get almost 10 kHz
  SCB_CLKOUTCLKDIV = 30;


#else


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


#endif
}

void rbInit() {
#if !HW_IS_PROTOTYPE
  RB_HB0_IO &= ~IOCON_SWDIO_PIO1_3_FUNC_MASK;
  RB_HB0_IO |=  IOCON_SWDIO_PIO1_3_FUNC_GPIO;
  RB_HB1_IO &= ~IOCON_JTAG_TCK_PIO0_10_FUNC_MASK;
  RB_HB1_IO |=  IOCON_JTAG_TCK_PIO0_10_FUNC_GPIO;
#endif

  struct {
    int port;
    int pin;
    uint32_t volatile *reg;
    gpioPullupMode_t mode;
  } const input_pins[] = {
#if HW_IS_PROTOTYPE
    { RB_BTN0    , &RB_BTN0_IO    , gpioPullupMode_PullDown },
    { RB_BTN1    , &RB_BTN1_IO    , gpioPullupMode_PullDown },
    { RB_BTN2    , &RB_BTN2_IO    , gpioPullupMode_PullDown },
    { RB_BTN3    , &RB_BTN3_IO    , gpioPullupMode_PullDown },
    { RB_BTN4    , &RB_BTN4_IO    , gpioPullupMode_PullDown },
    { RB_BTN_A   , &RB_BTN_A_IO   , gpioPullupMode_PullDown },
    { RB_BTN_B   , &RB_BTN_B_IO   , gpioPullupMode_PullDown },
#else
    { RB_BTN0    , &RB_BTN0_IO    , gpioPullupMode_PullUp },
    { RB_BTN1    , &RB_BTN1_IO    , gpioPullupMode_PullUp },
    { RB_BTN2    , &RB_BTN2_IO    , gpioPullupMode_PullUp },
    { RB_BTN3    , &RB_BTN3_IO    , gpioPullupMode_PullUp },
    { RB_BTN4    , &RB_BTN4_IO    , gpioPullupMode_PullUp },
    { RB_HB0     , &RB_HB0_IO     , gpioPullupMode_PullUp },
    { RB_HB1     , &RB_HB1_IO     , gpioPullupMode_PullUp },
    { RB_PWR_CHRG, &RB_PWR_CHRG_IO, gpioPullupMode_PullUp }
#endif
  };
    
  for(int i = 0; i < ARRAY_SIZE(input_pins); ++i) {
    gpioSetDir(input_pins[i].port, input_pins[i].pin, gpioDirection_Input);
    gpioSetPullup(input_pins[i].reg, input_pins[i].mode);
  }

#if HW_IS_PROTOTYPE
  IOCON_JTAG_TMS_PIO1_0 &= ~(IOCON_JTAG_TMS_PIO1_0_FUNC_MASK);
  IOCON_JTAG_TMS_PIO1_0 |=   IOCON_JTAG_TMS_PIO1_0_FUNC_GPIO;
#else
  // LED3 zur Bestimmung der Umgebungshelligkeit.
  gpioSetDir(RB_LED3, gpioDirection_Input);
  RB_LED3_IO = (RB_LED3_IO & IOCON_PIO1_11_FUNC_MASK) | IOCON_PIO1_11_FUNC_AD7;

  // prepare LEDs
  IOCON_JTAG_TDI_PIO0_11 &= ~IOCON_JTAG_TDI_PIO0_11_FUNC_MASK;
  IOCON_JTAG_TDI_PIO0_11 |=  IOCON_JTAG_TDI_PIO0_11_FUNC_GPIO;
#endif

  struct {
    int port;
    int pin;
    int value;
  } const output_pins[] = {
    { RB_PWR_GOOD, 0 },
    { USB_CONNECT, 1 },
    { RB_LCD_CS  , 1 },
#if !HW_IS_PROTOTYPE
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
#endif
  };

  for(int i = 0; i < ARRAY_SIZE(output_pins); ++i) {
    gpioSetDir  (output_pins[i].port, output_pins[i].pin, gpioDirection_Output);
    gpioSetValue(output_pins[i].port, output_pins[i].pin, output_pins[i].value);
  }

#if !HW_IS_PROTOTYPE
  // Set P0.0 to GPIO
  RB_PWR_LCDBL_IO &= ~RB_PWR_LCDBL_IO_FUNC_MASK;
  RB_PWR_LCDBL_IO |=  RB_PWR_LCDBL_IO_FUNC_GPIO;

  gpioSetDir   ( RB_PWR_LCDBL   , gpioDirection_Input);
  gpioSetPullup(&RB_PWR_LCDBL_IO, gpioPullupMode_Inactive);
#endif

  backlightInit();
}


/**************************************************************************/
/*!
    Main program entry point.  After reset, normal code execution will
    begin here.
*/
/**************************************************************************/
int main(void)
{
  // Configure cpu and mandatory peripherals
  //systemInit();

  cpuInit();
  systickInit(CFG_SYSTICK_DELAY_IN_MS);

  // pmuInit();
  // adcInit();
  rbInit();

  //  usbMSCInit();

  badge_display_init();

  /*
  gpioSetDir(0, 11, gpioDirection_Output);
  gpioSetValue(0, 11, 0);


  {
    badge_framebuffer fb;
    int res = 0;
    FATFS fatvol;

    if(FR_OK == f_mount(0, &fatvol)) {
      FIL fil;
      if(FR_OK == (res = f_open(&fil, "sshot.dat", FA_OPEN_EXISTING | FA_READ))) {
        UINT readbytes;

        if(FR_OK != f_read(&fil, &fb, sizeof(fb), &readbytes)) {
          gpioSetDir(RB_LED3, gpioDirection_Output);
          gpioSetValue(RB_LED3, 1);
        }

        f_close(&fil);
      } else {
        fb.data[0][0] = res;
        gpioSetDir(RB_LED1, gpioDirection_Output);
        gpioSetValue(RB_LED1, 1);
      }
    } else {
      gpioSetDir(RB_LED2, gpioDirection_Output);
      gpioSetValue(RB_LED2, 1);
    }

    badge_framebuffer_flush(&fb);
  }
  */

  badge_event_start();
  /*
  for(;;) {
    if(JUMPNRUN_ERROR == jumpnrun_play("smb.lvl")) {
      break;
    }
  }
  */

  uint8_t buttons = 0;

  for(uint8_t i = 0; ; ++i) {
    badge_event_t event = badge_event_wait();

    //    SCB_CLKOUTCLKDIV = i;

    switch(badge_event_type(event)) {
    case BADGE_EVENT_USER_INPUT: {
      buttons = badge_event_current_input_state();
      break;
    }
    case BADGE_EVENT_GAME_TICK: {
      badge_sprite const sp = { 4, 4, (uint8_t const *) "\xff\xff" };
      badge_framebuffer fb = { { { 0xcc } } };

      /*
      for(int i = 0; i < 9 * 96; ++i) {
	fb.data[0][i] = 0xbb;
      }
      */

      if(buttons & BADGE_EVENT_KEY_UP)    { badge_framebuffer_blt(&fb, 30, 10, &sp, 0); }
      if(buttons & BADGE_EVENT_KEY_DOWN)  { badge_framebuffer_blt(&fb, 30, 50, &sp, 0); }
      if(buttons & BADGE_EVENT_KEY_LEFT)  { badge_framebuffer_blt(&fb, 10, 30, &sp, 0); }
      if(buttons & BADGE_EVENT_KEY_RIGHT) { badge_framebuffer_blt(&fb, 50, 30, &sp, 0); }
      if(buttons & BADGE_EVENT_KEY_CENTER){ badge_framebuffer_blt(&fb, 30, 30, &sp, 0); }
      if(buttons & BADGE_EVENT_KEY_BTN_A) { badge_framebuffer_blt(&fb, 70, 10, &sp, 0); }
      if(buttons & BADGE_EVENT_KEY_BTN_B) { badge_framebuffer_blt(&fb, 70, 50, &sp, 0); }

      badge_framebuffer_flush(&fb);
      break;
    }
    }
  }

  return 0;
}
