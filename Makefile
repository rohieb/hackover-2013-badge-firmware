##########################################################################
# User configuration and firmware specific object files	
##########################################################################

# The target, flash and ram of the LPC1xxx microprocessor.
# Use for the target the value: LPC11xx, LPC13xx or LPC17xx
TARGET = LPC13xx
FLASH = 32K
SRAM = 8K

# For USB HID support the LPC134x reserves 384 bytes from the sram,
# if you don't want to use the USB features, just use 0 here.
SRAM_USB = 384

##########################################################################
# Debug settings
##########################################################################

# Set DEBUGBUILD to 'TRUE' for full debugging (larger, slower binaries), 
# or to 'FALSE' for release builds (smallest, fastest binaries)
DEBUGBUILD = FALSE

##########################################################################
# IDE Flags (Keeps various IDEs happy)
##########################################################################

OPTDEFINES = -D __NEWLIB__

##########################################################################
# Project-specific files 
##########################################################################

SRCS = \
  badge/main.c \
  badge/ui/display.c \
  badge/ui/sprite.c

SRCS += \
  dataflash/at45db041d.c \
  dataflash/iobase.c \
  dataflash/diskio.c

SRCS += \
  project/commands.c

SRCS += \
  project/commands/cmd_chibi_addr.c \
  project/commands/cmd_chibi_tx.c \
  project/commands/cmd_i2ceeprom_read.c \
  project/commands/cmd_i2ceeprom_write.c \
  project/commands/cmd_lm75b_gettemp.c \
  project/commands/cmd_reset.c \
  project/commands/cmd_sd_dir.c \
  project/commands/cmd_sysinfo.c \
  project/commands/cmd_uart.c \
  project/commands/cmd_pwm.c

SRCS += \
  project/commands/drawing/cmd_backlight.c \
  project/commands/drawing/cmd_bmp.c \
  project/commands/drawing/cmd_button.c \
  project/commands/drawing/cmd_calibrate.c \
  project/commands/drawing/cmd_circle.c \
  project/commands/drawing/cmd_clear.c \
  project/commands/drawing/cmd_line.c \
  project/commands/drawing/cmd_orientation.c \
  project/commands/drawing/cmd_pixel.c \
  project/commands/drawing/cmd_progress.c \
  project/commands/drawing/cmd_rectangle.c \
  project/commands/drawing/cmd_roundedcorner.c \
  project/commands/drawing/cmd_text.c \
  project/commands/drawing/cmd_textw.c \
  project/commands/drawing/cmd_tsthreshhold.c \
  project/commands/drawing/cmd_tswait.c \
  project/commands/drawing/cmd_triangle.c

##########################################################################
# Optional driver files 
##########################################################################

# Chibi Light-Weight Wireless Stack (AT86RF212)
SRCS += \
  drivers/rf/chibi/chb.c \
  drivers/rf/chibi/chb_buf.c \
  drivers/rf/chibi/chb_drvr.c \
  drivers/rf/chibi/chb_eeprom.c \
  drivers/rf/chibi/chb_spi.c

# 4K EEPROM
SRCS += \
  drivers/storage/eeprom/eeprom.c \
  drivers/storage/eeprom/mcp24aa/mcp24aa.c

# LM75B temperature sensor
SRCS += \
  drivers/sensors/lm75b/lm75b.c

# ISL12022M RTC
SRCS += \
  drivers/rtc/isl12022m/isl12022m.c

# TFT LCD support
SRCS += \
  drivers/displays/tft/drawing.c \
  drivers/displays/tft/touchscreen.c \
  drivers/displays/tft/colors.c \
  drivers/displays/tft/theme.c \
  drivers/displays/tft/bmp.c

# GUI Controls
SRCS += \
  drivers/displays/tft/controls/button.c \
  drivers/displays/tft/controls/hsbchart.c \
  drivers/displays/tft/controls/huechart.c \
  drivers/displays/tft/controls/label.c \
  drivers/displays/tft/controls/labelcentered.c \
  drivers/displays/tft/controls/progressbar.c

# Bitmap (non-AA) fonts
SRCS += \
  drivers/displays/tft/fonts.c \
  drivers/displays/tft/fonts/dejavusans9.c \
  drivers/displays/tft/fonts/dejavusansbold9.c \
  drivers/displays/tft/fonts/dejavusanscondensed9.c \
  drivers/displays/tft/fonts/dejavusansmono8.c \
  drivers/displays/tft/fonts/dejavusansmonobold8.c \
  drivers/displays/tft/fonts/verdana9.c \
  drivers/displays/tft/fonts/verdana14.c \
  drivers/displays/tft/fonts/verdanabold14.c

# Anti-aliased fonts
SRCS += \
  drivers/displays/tft/aafonts.c \
  drivers/displays/tft/aafonts/aa2/DejaVuSansCondensed14_AA2.c \
  drivers/displays/tft/aafonts/aa2/DejaVuSansCondensedBold14_AA2.c \
  drivers/displays/tft/aafonts/aa2/DejaVuSansMono10_AA2.c \
  drivers/displays/tft/aafonts/aa2/DejaVuSansMono13_AA2.c \
  drivers/displays/tft/aafonts/aa2/DejaVuSansMono14_AA2.c

# LCD Driver (Only one can be included at a time!)
# SRCS += drivers/displays/tft/hw/hx8340b.c
# SRCS += drivers/displays/tft/hw/hx8347d.c
# SRCS += drivers/displays/tft/hw/ILI9328.c
# SRCS += drivers/displays/tft/hw/ILI9325.c
# SRCS += drivers/displays/tft/hw/ssd1331.c
# SRCS += drivers/displays/tft/hw/ssd1351.c
# SRCS += drivers/displays/tft/hw/st7735.c
# SRCS += drivers/displays/tft/hw/st7783.c

# Bitmap/Monochrome LCD support (ST7565, SSD1306, etc.)
SRCS += \
  drivers/displays/smallfonts.c \
  drivers/displays/bitmap/sharpmem/sharpmem.c \
  drivers/displays/bitmap/st7565/st7565.c \
  drivers/displays/bitmap/ssd1306/ssd1306.c

#Character Displays (VFD text displays, etc.)
SRCS += \
  drivers/displays/character/samsung_20T202DA2JA/samsung_20T202DA2JA.c

# ChaN FatFS and SD card support
SRCS += \
  drivers/fatfs/ff.c 
# drivers/fatfs/mmc.c

# Motors
SRCS += \
  drivers/motor/stepper/stepper.c

# RSA Encryption/Descryption
SRCS += \
  drivers/rsa/rsa.c

# DAC
SRCS += \
  drivers/dac/mcp4725/mcp4725.c \
  drivers/dac/mcp4901/mcp4901.c

# RFID/NFC
SRCS += \
  drivers/rf/pn532/pn532.c \
  drivers/rf/pn532/pn532_bus_i2c.c \
  drivers/rf/pn532/pn532_bus_uart.c \
  drivers/rf/pn532/helpers/pn532_mifare_classic.c \
  drivers/rf/pn532/helpers/pn532_mifare_ultralight.c

# TAOS Light Sensors
SRCS += \
  drivers/sensors/tcs3414/tcs3414.c \
  drivers/sensors/tsl2561/tsl2561.c

# SPI Flash
SRCS += \
  drivers/storage/spiflash/w25q16bv/w25q16bv.c

# FM Radio
SRCS += \
  drivers/audio/tea5767/tea5767.c

# IN219 Current Sensor
SRCS += \
  drivers/sensors/ina219/ina219.c

# MPL115A2 Barometric Pressure Sensor
SRCS += \
  drivers/sensors/mpl115a2/mpl115a2.c

# ADS1015 12-bit ADC
SRCS += \
  drivers/adc/ads1015/ads1015.c

##########################################################################
# Library files 
##########################################################################

SRCS += \
  core/adc/adc.c \
  core/cmd/cmd.c \
  core/cpu/cpu.c \
  core/gpio/gpio.c \
  core/i2c/i2c.c \
  core/iap/iap.c \
  core/libc/stdio.c \
  core/libc/string.c \
  core/pmu/pmu.c \
  core/pwm/pwm.c \
  core/ssp/ssp.c \
  core/systick/systick.c \
  core/timer16/timer16.c \
  core/timer32/timer32.c \
  core/uart/uart_buf.c \
  core/uart/uart.c \
  core/usbcdc/cdc_buf.c \
  core/usbcdc/cdcuser.c \
  core/usbcdc/usbcore.c \
  core/usbcdc/usbdesc.c \
  core/usbcdc/usbhw.c \
  core/usbcdc/usbuser.c \
  core/usbhid-rom/usbconfig.c \
  core/usbhid-rom/usbhid.c \
  core/usbhid-rom/usbmsc.c \
  core/wdt/wdt.c

##########################################################################
# GNU GCC compiler prefix and location
##########################################################################

CROSS_COMPILE = arm-none-eabi-
AS = $(CROSS_COMPILE)gcc
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
SIZE = $(CROSS_COMPILE)size
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
OUTFILE = firmware
LPCRC = ./lpcrc

##########################################################################
# GNU GCC compiler flags
##########################################################################
ROOT_PATH = .
INCLUDE_PATHS = -I$(ROOT_PATH) -I$(ROOT_PATH)/project

##########################################################################
# Startup files
##########################################################################

LD_PATH = lpc1xxx
LD_SCRIPT = $(LD_PATH)/linkscript.ld
LD_TEMP = $(LD_PATH)/memory.ld

ifeq (LPC11xx,$(TARGET))
  CORTEX_TYPE=m0
else
  CORTEX_TYPE=m3
endif

CPU_TYPE = cortex-$(CORTEX_TYPE)

SRCS += \
  lpc1xxx/$(TARGET)_handlers.c \
  lpc1xxx/LPC1xxx_startup.c

OBJS = $(SRCS:%.c=%.o)
DEPS = $(SRCS:%.c=%.dep)

##########################################################################
# Compiler settings, parameters and flags
##########################################################################
ifeq (TRUE,$(DEBUGBUILD))
  CFLAGS  = -c -g -O0 $(INCLUDE_PATHS) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -DTARGET=$(TARGET) -fno-builtin $(OPTDEFINES) -std=c99
  ASFLAGS = -c -g -O0 $(INCLUDE_PATHS) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -D__ASSEMBLY__ -x assembler-with-cpp
else
  CFLAGS  = -c -g -Os $(INCLUDE_PATHS) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -DTARGET=$(TARGET) -fno-builtin $(OPTDEFINES) -std=c99
  ASFLAGS = -c -g -Os $(INCLUDE_PATHS) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -D__ASSEMBLY__ -x assembler-with-cpp
endif

LDFLAGS = -nostartfiles -mthumb -mcpu=$(CPU_TYPE) -Wl,--gc-sections
LDLIBS  = -lm
OCFLAGS = --strip-unneeded

all: dep size $(OUTFILE).bin $(OUTFILE).hex

dep: $(DEPS)

$(DEPS) : %.dep : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MM $< -MT $@ -MT $(<:%.c=%.o) -MF $@

sinclude $(DEPS)

$(LD_TEMP): 
	-@echo "MEMORY" > $(LD_TEMP)
	-@echo "{" >> $(LD_TEMP)
	-@echo "  flash(rx): ORIGIN = 0x00000000, LENGTH = $(FLASH)" >> $(LD_TEMP)
	-@echo "  sram(rwx): ORIGIN = 0x10000000+$(SRAM_USB), LENGTH = $(SRAM)-$(SRAM_USB)" >> $(LD_TEMP)
	-@echo "}" >> $(LD_TEMP)
	-@echo "INCLUDE $(LD_SCRIPT)" >> $(LD_TEMP)

$(OUTFILE).elf: $(OBJS) $(LD_TEMP)
	$(LD) $(LDFLAGS) -T $(LD_TEMP) -o $(OUTFILE).elf $(OBJS) $(LDLIBS)

$(OUTFILE).bin: $(OUTFILE).elf
	$(OBJCOPY) $(OCFLAGS) -O binary $(OUTFILE).elf $(OUTFILE).bin
	$(LPCRC) $(OUTFILE).bin

$(OUTFILE).hex: $(OUTFILE).elf
	$(OBJCOPY) $(OCFLAGS) -O ihex $(OUTFILE).elf $(OUTFILE).hex

size: $(OUTFILE).elf
	$(SIZE) $(OUTFILE).elf

clean:
	rm -f $(OBJS) $(LD_TEMP) $(OUTFILE).elf $(OUTFILE).bin $(OUTFILE).hex

distclean: clean
	rm -f $(DEPS)

.PHONY: all dep size clean distclean
