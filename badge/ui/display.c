#include "display.h"

#include <core/ssp/ssp.h>
#include <core/gpio/gpio.h>
#include <core/systick/systick.h>
#include <sysdefs.h>
#include <lpc134x.h>

#ifdef R0KET

#include <r0ketports.h>

#define CS_LOW()   gpioSetValue(RB_LCD_CS , 0)
#define CS_HIGH()  gpioSetValue(RB_LCD_CS , 1)
#define RST_LOW()  gpioSetValue(RB_LCD_RST, 0)
#define RST_HIGH() gpioSetValue(RB_LCD_RST, 1)

#else

#include <badge/pinconfig.h>

#define CS_LOW()   gpioSetValue(HOB_PORT(HOB_LCD_CS ), HOB_PIN(HOB_LCD_CS ), 0)
#define CS_HIGH()  gpioSetValue(HOB_PORT(HOB_LCD_CS ), HOB_PIN(HOB_LCD_CS ), 1)
#define RST_LOW()  gpioSetValue(HOB_PORT(HOB_LCD_RST), HOB_PIN(HOB_LCD_RST), 0)
#define RST_HIGH() gpioSetValue(HOB_PORT(HOB_LCD_RST), HOB_PIN(HOB_LCD_RST), 1)

#endif

#ifdef CFG_USBMSC
#include <core/usbhid-rom/usbmsc.h>

static uint32_t usbintstatus;
#endif

static void lcd_select() {
#ifdef CFG_USBMSC
  if(usbMSCenabled) {
    usbintstatus = USB_DEVINTEN;
    USB_DEVINTEN = 0;
  }
#endif

    /* the LCD requires 9-Bit frames */
  uint32_t configReg = ( SSP_SSP0CR0_DSS_9BIT     // Data size = 9-bit
                         | SSP_SSP0CR0_FRF_SPI    // Frame format = SPI
                         | SSP_SSP0CR0_SCR_8);    // Serial clock rate = 8
  SSP_SSP0CR0 = configReg;

  CS_LOW();
}

static void lcd_deselect() {
  CS_HIGH();
  /* reset the bus to 8-Bit frames that everyone else uses */
  uint32_t configReg = ( SSP_SSP0CR0_DSS_8BIT     // Data size = 8-bit
                         | SSP_SSP0CR0_FRF_SPI    // Frame format = SPI
                         | SSP_SSP0CR0_SCR_8);    // Serial clock rate = 8
  SSP_SSP0CR0 = configReg;

#ifdef CFG_USBMSC
  if(usbMSCenabled) {
    USB_DEVINTEN = usbintstatus;
  }
#endif
}

static inline void lcd_write(uint16_t frame) {
  while ((SSP_SSP0SR & (SSP_SSP0SR_TNF_NOTFULL | SSP_SSP0SR_BSY_BUSY)) != SSP_SSP0SR_TNF_NOTFULL);
  SSP_SSP0DR = frame;
  while ((SSP_SSP0SR & (SSP_SSP0SR_BSY_BUSY|SSP_SSP0SR_RNE_NOTEMPTY)) != SSP_SSP0SR_RNE_NOTEMPTY);
    /* clear the FIFO */
  frame = SSP_SSP0DR;
}

static void lcd_write_command(uint8_t data) { lcd_write(         data); }
static void lcd_write_data   (uint8_t data) { lcd_write(0x0100 | data); }

void badge_display_init(void) {
  sspInit(0, sspClockPolarity_Low, sspClockPhase_RisingEdge);

  CS_HIGH();
  RST_HIGH();

  systickDelay(100);
  RST_LOW();
  systickDelay(100);
  RST_HIGH();
  systickDelay(100);
  /*
  int id = lcdRead(220);

  if(id == 14) {
    gpioSetDir(1, 7, gpioDirection_Output);
    gpioSetValue(1, 7, 1);
  }
  */

/* Small Nokia 1200 LCD docs:
 *           clear/ set
 *  on       0xae / 0xaf
 *  invert   0xa6 / 0xa7
 *  mirror-x 0xA0 / 0xA1
 *  mirror-y 0xc7 / 0xc8
 *
 *  0x20+x contrast (0=black - 0x2e)
 *  0x40+x offset in rows from top (-0x7f)
 *  0x80+x contrast? (0=black -0x9f?)
 *  0xd0+x black lines from top? (-0xdf?)
 *
 */
  lcd_select();

  /* Decoded:
   * E2: Internal reset
   * AF: Display on/off: DON = 1
   * A1: undefined?
   * A4: all on/normal: DAL = 0
   * 2F: charge pump on/off: PC = 1
   * B0: set y address: Y[0-3] = 0
   * 10: set x address (upper bits): X[6-4] = 0
   */
  static uint8_t const initseq[]= { 0xE2, 0xAF, // Display ON
				    0xA1,       // Mirror-X
                                    //0xc8, // mirror-y
                                    0xa7, // invert (1 = black)
                                    0xA4, 0x2F,
				    // 0x9f, 0x24
                                    0xB0, 0x10,
  };
  for(uint8_t i = 0; i < sizeof(initseq); ++i){
    lcd_write_command(initseq[i]);
    systickDelay(5);
  }

  lcd_deselect();
}

void badge_framebuffer_flush(badge_framebuffer const *fb) {
  lcd_select();

  lcd_write_command(0xb0);
  lcd_write_command(0x10);
  lcd_write_command(0x00);


  for(int i = 0; i < BADGE_DISPLAY_STRIPE_COUNT * BADGE_DISPLAY_WIDTH; ++i) {
    lcd_write_data(fb->data[0][i]);
  }

  lcd_deselect();
}
