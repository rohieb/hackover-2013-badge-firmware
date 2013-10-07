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
#include "core/gpio/gpio.h"
#include "core/systick/systick.h"
#include "core/usbhid-rom/usbmsc.h"

#ifdef CFG_INTERFACE
  #include "core/cmd/cmd.h"
#endif

#include "lcd/display.h"
#include "lcd/sprite.h"

#include "r0ketports.h"
#include "drivers/fatfs/ff.h"

uint8_t getInputRaw(void) {
    uint8_t result = BTN_NONE;

    if (gpioGetValue(RB_BTN3)==0) {
        result |= BTN_UP;
    }

    if (gpioGetValue(RB_BTN2)==0) {
        result |= BTN_DOWN;
    }

    if (gpioGetValue(RB_BTN4)==0) {
        result |= BTN_ENTER;
    }

    if (gpioGetValue(RB_BTN0)==0) {
        result |= BTN_LEFT;
    }

    if (gpioGetValue(RB_BTN1)==0) {
        result |= BTN_RIGHT;
    }

    return result;
}

void backlightInit(void) {
  /* Enable the clock for CT16B1 */
  SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_CT16B1);

  /* Configure PIO1.9 as Timer1_16 MAT0 Output */
  IOCON_PIO1_9 &= ~IOCON_PIO1_9_FUNC_MASK;
  IOCON_PIO1_9 |= IOCON_PIO1_9_FUNC_CT16B1_MAT0;

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
    // TODO FIXME special port disable ? LEDs BTNs ?

    // prepare power
    // TODO FIXME more power init needed ? chrg + volt input ?
    // enable external vcc
    gpioSetDir(RB_PWR_GOOD, gpioDirection_Output);
    gpioSetValue (RB_PWR_GOOD, 0);

    // Disable USB Connect (we don't want USB by default)
    gpioSetDir(USB_CONNECT, gpioDirection_Output);
    gpioSetValue(USB_CONNECT, 1);

    static uint8_t ports[] = { RB_BTN0, RB_BTN1, RB_BTN2, RB_BTN3, RB_BTN4,
                        RB_LED0, RB_LED1, RB_LED2,
                        RB_SPI_SS0, RB_SPI_SS1, RB_SPI_SS2,
                        RB_SPI_SS3, RB_SPI_SS4, RB_SPI_SS5,
                        RB_HB0, RB_HB1, RB_HB2,
                        RB_HB3, RB_HB4, RB_HB5};

    volatile uint32_t * regs[] = {&RB_BTN0_IO, &RB_BTN1_IO, &RB_BTN2_IO,
                                  &RB_BTN3_IO, &RB_BTN4_IO};

    int i = 0;
    while( i<10 ){
        gpioSetDir(ports[i], ports[i+1], gpioDirection_Input);
        gpioSetPullup(regs[i/2], gpioPullupMode_PullUp);
        i+=2;
    }

    // prepate chrg_stat
    gpioSetDir(RB_PWR_CHRG, gpioDirection_Input);
    gpioSetPullup (&RB_PWR_CHRG_IO, gpioPullupMode_PullUp);

    gpioSetDir(RB_LED3, gpioDirection_Input);
    IOCON_PIO1_11 = 0x41;

    // prepare LEDs
    IOCON_JTAG_TDI_PIO0_11 &= ~IOCON_JTAG_TDI_PIO0_11_FUNC_MASK;
    IOCON_JTAG_TDI_PIO0_11 |= IOCON_JTAG_TDI_PIO0_11_FUNC_GPIO;

    while( i<16 ){
        gpioSetDir(ports[i],ports[i+1], gpioDirection_Output);
        gpioSetValue (ports[i], ports[i+1], 0);
        i+=2;
    }

    // Set LED3 to ?
    IOCON_PIO1_11 = 0x41;

    // prepare lcd
    // TODO FIXME more init needed ?
    gpioSetDir(RB_LCD_BL, gpioDirection_Output);
    gpioSetValue (RB_LCD_BL, 0);

    // Set P0.0 to GPIO
    RB_PWR_LCDBL_IO&= ~RB_PWR_LCDBL_IO_FUNC_MASK;
    RB_PWR_LCDBL_IO|= RB_PWR_LCDBL_IO_FUNC_GPIO;
    gpioSetDir(RB_PWR_LCDBL, gpioDirection_Input);
    gpioSetPullup(&RB_PWR_LCDBL_IO, gpioPullupMode_Inactive);

    // prepare SPI/SS
    // TODO FIXME init miso/mosi/sck somehow ?
    // prepare hackerbus
    while(i<sizeof(ports)){
        gpioSetDir(ports[i],ports[i+1], gpioDirection_Output);
        gpioSetValue (ports[i], ports[i+1], 1);
        i+=2;
    }

    backlightInit();
    //font=&Font_7x8;
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
  //gpioInit();
  // pmuInit();
  //  adcInit();
  rbInit();

  badge_display_init();
  gpioSetDir(0, 11, gpioDirection_Output);

  badge_framebuffer fb = {
    {
      {
        1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96
      }, {
        1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96
      }, {
        1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96
      }, {
        1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96
      }, {
        1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96
      }, {
        1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96
      }, {
        1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96
      }, {
        1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96
      }, {
        1,   2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96
      }
    }
  };

  //  badge_framebuffer_flush(&fb);

  usbMSCInit();

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

  for(uint8_t i = 0; ; ++i) {
    gpioSetValue(0, 11, i & 1);

    uint8_t buttons = getInputRaw();
    memset(&fb, 0, sizeof(fb));

    badge_sprite const sp = { 4, 4, (uint8_t const *) "\xff\xff" };

    if(buttons & BTN_UP)    { badge_framebuffer_blt(&fb, 30, 10, &sp, 0); }
    if(buttons & BTN_DOWN)  { badge_framebuffer_blt(&fb, 30, 50, &sp, 0); }
    if(buttons & BTN_LEFT)  { badge_framebuffer_blt(&fb, 10, 30, &sp, 0); }
    if(buttons & BTN_RIGHT) { badge_framebuffer_blt(&fb, 50, 30, &sp, 0); }

    badge_framebuffer_flush(&fb);

    systickDelay(500);
  }

  return 0;
}
