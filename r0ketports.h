#ifndef INCLUDED_R0KET_PORTS_H
#define INCLUDED_R0KET_PORTS_H

#define HW_IS_PROTOTYPE 1

// LED
#define RB_LED0			    0,11
#define RB_LED0_IO          IOCON_PIO0_11
#define RB_LED1			    1,7
#define RB_LED1_IO          IOCON_PIO1_7
#define RB_LED2			    1,6
#define RB_LED2_IO          IOCON_PIO1_6
#define RB_LED3			    1,11
#define RB_LED3_IO          IOCON_PIO1_11

// Infrared
//#define RB_IROUT		    1,5

//#define RB_IRIN			1,8
//#define RB_IRIN_IO		IOCON_PIO1_8


// Buttons
#if HW_IS_PROTOTYPE

// TODO
// FIXME:
// order of buttons is without sense!
#define RB_BTN0			    1,5
#define RB_BTN0_IO		    IOCON_PIO0_1

#define RB_BTN1			    1,6
#define RB_BTN1_IO		    IOCON_PIO2_9

#define RB_BTN2			    3,2
#define RB_BTN2_IO		    IOCON_PIO2_6

#define RB_BTN3			    3,3
#define RB_BTN3_IO		    IOCON_PIO3_3

#define RB_BTN4			    2,7
#define RB_BTN4_IO		    IOCON_PIO2_7

// A and B buttons on the hackover badge only
#define RB_BTN_A            1,11
#define RB_BTN_A_IO         IOCON_PIO1_11

#define RB_BTN_B            1,4
#define RB_BTN_B_IO         IOCON_PIO1_4

#else

// TODO: XXX
// use alternate buttons for the board

#define RB_BTN0			    0,1
#define RB_BTN0_IO		    IOCON_PIO0_1

#define RB_BTN1			    2,9
#define RB_BTN1_IO		    IOCON_PIO2_9

#define RB_BTN2			    2,6
#define RB_BTN2_IO		    IOCON_PIO2_6

#define RB_BTN3			    3,3
#define RB_BTN3_IO		    IOCON_PIO3_3

#define RB_BTN4			    2,7
#define RB_BTN4_IO		    IOCON_PIO2_7


#endif


// LCD
#if HW_IS_PROTOTYPE

// backlight pin is used     in another way for prototype
#define RB_LCD_CS	        1,0
#define RB_LCD_RST		    1,1

#else

#define RB_LCD_BL		    1,9
#define RB_LCD_CS		    2,1
#define RB_LCD_RST		2,2

#endif


// I2C
#define RB_I2C_SCL		    0,4

#define RB_I2C_SCA		    0,5


// SPI
#define RB_SPI_MISO		    0,8

#define RB_SPI_MOSI		    0,9

#define RB_SPI_SCK		    2,11

#define RB_SPI_CS_DF	    2,0

#define RB_SPI_SS0		    2,5

#define RB_SPI_SS1		    2,4

#define RB_SPI_SS2		    2,8

#define RB_SPI_SS3		    3,2

#define RB_SPI_SS4		    3,1

#define RB_SPI_SS5		    2,10


// Power
#define RB_PWR_VOLT		    1,0

#define RB_PWR_GOOD		    1,8

#define RB_PWR_CHRG		    2,3
#define RB_PWR_CHRG_IO	    IOCON_PIO2_3

#define RB_PWR_LCDBL        0,0
#define RB_PWR_LCDBL_IO     IOCON_nRESET_PIO0_0
#define RB_PWR_LCDBL_IO_FUNC_MASK IOCON_nRESET_PIO0_0_FUNC_MASK
#define RB_PWR_LCDBL_IO_FUNC_GPIO IOCON_nRESET_PIO0_0_FUNC_GPIO

// Hackerbus
#define RB_HB0			    1,3
#define RB_HB0_IO		    IOCON_SWDIO_PIO1_3

#define RB_HB1			    0,10
#define RB_HB1_IO		    IOCON_JTAG_TCK_PIO0_10

#define RB_HB2			    1,1
#define RB_HB2_IO		    IOCON_PIO1_1

#define RB_HB3			    0,2
#define RB_HB3_IO		    IOCON_PIO0_2

#define RB_HB4			    1,4
#define RB_HB4_IO		    IOCON_PIO1_4

#define RB_HB5			    1,2
#define RB_HB5_IO		    IOCON_PIO1_2

// Funk
#if HW_IS_PROTOTYPE

#define RB_NRF_CE		    2,10
#define RB_NRF_CE_IO		IOCON_PIO2_10
#define RB_SPI_NRF_CS		2,9
#define RB_SPI_NRF_CS_IO	IOCON_PIO2_9

// TODO: this pin is new. support must be added for configuration in main.c
#define RB_SPI_NRF_IRQ		2,2
#define RB_SPI_NRF_IRQ_IO	IOCON_PIO2_2

#else

#define RB_NRF_CE		    1,5
#define RB_NRF_CE_IO		IOCON_PIO1_5
#define RB_SPI_NRF_CS		1,10
#define RB_SPI_NRF_CS_IO	IOCON_PIO1_10

#endif

// Misc
#define RB_BUSINT		    3,0
#define RB_BUSINT_IO		IOCON_PIO3_0

#define RB_USB_VBUS		    0,3

#define RB_EEPROM_ADDR 		0xA0

#define USB_CONNECT         0,6


#define BTN_NONE 0
#define BTN_UP   (1<<0)
#define BTN_DOWN (1<<1)
#define BTN_LEFT (1<<2)
#define BTN_RIGHT (1<<3)
#define BTN_ENTER (1<<4)

#endif

