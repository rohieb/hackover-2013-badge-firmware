#include "level.h"

#ifndef __thumb__

typedef enum {
  FR_OK,
  FR_SOME_ERROR
} FRESULT;

typedef size_t UINT;

static FRESULT f_read (FIL *fd, void *buf, UINT bufsize, UINT *bytes) {
  *bytes = fread(buf, 1, bufsize, fd);
  return *bytes == 0 ? FR_SOME_ERROR : FR_OK;
}

#endif

int gladio_load_level_header_from_file(gladio_level *dest, FIL *fd) {
  UINT count;

  if(FR_OK != f_read(fd, &dest->len, sizeof(dest->len), &count) || count != sizeof(dest->len)) {
    return GLADIO_ERROR;
  }

  return GLADIO_SUCCESS;
}

int gladio_load_level_from_file(gladio_level *dest, FIL *fd) {
  UINT count;

  if(FR_OK != f_read(fd,
                     dest->specs,
                     dest->len * sizeof(*dest->specs),
                     &count)
     || count != dest->len * sizeof(*dest->specs))
  {
    return GLADIO_ERROR;
  }

  return GLADIO_SUCCESS;
}
