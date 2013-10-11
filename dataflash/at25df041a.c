#include "projectconfig.h"
#include "diskio.h"
#include "iobase.h"
#include "core/ssp/ssp.h"

#include <r0ketports.h>

/* Opcodes */
enum {
  OP_READARRAY        = 0x0b,
  OP_READARRAY_SLOW   = 0x03,

  OP_ERASE_BLOCK_4K   = 0x20,
  OP_ERASE_BLOCK_32K  = 0x52,
  OP_ERASE_BLOCK_64k  = 0xd8,
  OP_ERASE_CHIP       = 0x60,
  OP_ERASE_CHIP2      = 0xc7,

  OP_PROGRAM_PAGE     = 0x02,
  OP_PROGRAM_SEQ      = 0xad,
  OP_PROGRAM_SEQ2     = 0xaf,

  OP_WRITE_ENABLE     = 0x06,
  OP_WRITE_DISABLE    = 0x04,

  OP_SECTOR_PROTEXT   = 0x36,
  OP_SECTOR_UNPROTECT = 0x39,
  OP_SECTOR_STATUS    = 0x3c,

  OP_STATUS_READ      = 0x05,
  OP_STATUS_WRITE     = 0x01,

  OP_DEVICEID_READ    = 0x9f,

  OP_POWERDOWN        = 0xb9,
  OP_RESUME           = 0xab,
};

enum {
  STATUS_SPRL = 1 << 7, // sector protection registers locked
  STATUS_SPM  = 1 << 6, // sequential program mode
  STATUS_EPE  = 1 << 5, // erase/program error
  STATUS_WPP  = 1 << 4, // write protect pin
  STATUS_SWP  = 3 << 2, // software protection
  STATUS_WEL  = 1 << 1, // write enable latch
  STATUS_BSY  = 1 << 0, // ready/busy

  STATUS_SWP_NONE = 0,      //   no sectors are software-protected
  STATUS_SWP_SOME = 1 << 2, // some sectors are software-protected
  STATUS_SWP_ALL  = 3 << 2  //  all sectors are software-protected
};

enum {
  PAGE_MAX  = 2048,
  PAGE_SIZE = 256,

  SECTOR_SIZE_FATFS = 512
};

#define CS_LOW()    gpioSetValue(RB_SPI_CS_DF, 0)
#define CS_HIGH()   gpioSetValue(RB_SPI_CS_DF, 1)

static volatile DSTATUS status = STA_NOINIT;

static void wait_for_ready() {
  BYTE reg_status = 0xFF;

  CS_LOW();
  xmit_spi(OP_STATUS_READ);
  do {
    rcvr_spi_m((uint8_t *) &reg_status);
  } while (reg_status & STATUS_BSY);
  CS_HIGH();
}

static void dataflash_powerdown() {
  CS_LOW();
  xmit_spi(OP_POWERDOWN);
  CS_HIGH();
}

static void dataflash_resume() {
  CS_LOW();
  xmit_spi(OP_RESUME);
  CS_HIGH();
}

DSTATUS dataflash_initialize() {
  sspInit(0, sspClockPolarity_Low, sspClockPhase_RisingEdge);

  gpioSetDir(RB_SPI_CS_DF, gpioDirection_Output);
  dataflash_resume();
  status &= ~STA_NOINIT;

  return status;
}

DSTATUS dataflash_status() {
  return status;
}

DRESULT dataflash_random_read(BYTE *buff, DWORD offset, DWORD length) {
  if (!length) return RES_PARERR;
  if (status & STA_NOINIT) return RES_NOTRDY;
  if (offset + length > PAGE_MAX * PAGE_SIZE) return RES_PARERR;

  wait_for_ready();
  CS_LOW();
  xmit_spi(OP_READARRAY);
  xmit_spi((BYTE)(offset >> 16));
  xmit_spi((BYTE)(offset >> 8));
  xmit_spi((BYTE)offset);
  xmit_spi(0x00); // follow up with don't care byte

  do {
    rcvr_spi_m(buff++);
  } while (--length);
  CS_HIGH();

  return RES_OK;
}

DRESULT dataflash_read(BYTE *buff, DWORD sector, BYTE count) {
  return dataflash_random_read(buff, sector * SECTOR_SIZE_FATFS, count * SECTOR_SIZE_FATFS);
}

#if _READONLY == 0
DRESULT dataflash_random_write(const BYTE *buff, DWORD offset, DWORD length) {
  if (!length) return RES_PARERR;
  if (status & STA_NOINIT) return RES_NOTRDY;
  if (offset + length > PAGE_MAX * PAGE_SIZE) return RES_PARERR;

  do {
    wait_for_ready();

    DWORD pageaddr = (offset / PAGE_SIZE) * PAGE_SIZE;
    DWORD remaining = PAGE_SIZE - offset % PAGE_SIZE;

    if (remaining > length) {
      remaining = length;
    }

    length -= remaining;
    offset += remaining;

    CS_LOW();
    xmit_spi(OP_WRITE_ENABLE);
    CS_HIGH();

    // read page into the internal buffer
    CS_LOW();
    xmit_spi(OP_PROGRAM_PAGE);
    xmit_spi((BYTE)(pageaddr >> 16));
    xmit_spi((BYTE)(pageaddr >>  8));
    xmit_spi((BYTE) pageaddr       );

    do {
      xmit_spi(*buff++);
    } while (--remaining);

    CS_HIGH();
  } while (length);

  return RES_OK;
}

DRESULT dataflash_write(const BYTE *buff, DWORD sector, BYTE count) {
  return dataflash_random_write(buff, sector * SECTOR_SIZE_FATFS, count * SECTOR_SIZE_FATFS);
}
#endif /* _READONLY */

#if _USE_IOCTL != 0
DRESULT dataflash_ioctl(BYTE ctrl, void *buff) {
  DRESULT res;
  BYTE *ptr = buff;

  res = RES_ERROR;

  if (ctrl == CTRL_POWER) {
    switch (*ptr) {
    case 0: /* Sub control code == 0 (POWER_OFF) */
      dataflash_powerdown();
      res = RES_OK;
      break;
    case 1: /* Sub control code == 1 (POWER_ON) */
      dataflash_resume();
      res = RES_OK;
      break;
    case 2: /* Sub control code == 2 (POWER_GET) */
      // TODO: figure out a way to retrieve the powerstate
      *(ptr+1) = (BYTE)1;
      res = RES_OK;
      break;
    default :
      res = RES_PARERR;
    }
  } else {
    if (status & STA_NOINIT) return RES_NOTRDY;

    switch (ctrl) {
    case CTRL_SYNC:
      wait_for_ready();
      res = RES_OK;
      break;
    case GET_SECTOR_COUNT:
      // TODO: read from device ID register
      *(WORD*)buff = PAGE_MAX * PAGE_SIZE / SECTOR_SIZE_FATFS;
      res = RES_OK;
      break;
    case GET_SECTOR_SIZE:
      *(WORD*)buff = SECTOR_SIZE_FATFS;
      res = RES_OK;
      break;
    case GET_BLOCK_SIZE:
      *(WORD*)buff = 1;
      res = RES_OK;
      break;
    default:
      res = RES_PARERR;
    }
  }

  return res;
}
#endif /* _USE_IOCTL != 0 */

DWORD get_fattime () {
  return 0;
  /*
    struct tm* tm=mygmtime(getSeconds());
    DWORD t= (((tm->tm_year-80)<<9)|
            ((tm->tm_mon+1)<<5)|
            (tm->tm_mday))<<16 |
            ((tm->tm_hour<<11)|
            (tm->tm_min<<5)|
            (tm->tm_sec>>1));
    return t;
  */
}
