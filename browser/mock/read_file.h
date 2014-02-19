#ifndef INCLUDED_READ_FILE_H
#define INCLUDED_READ_FILE_H

#include <stddef.h>

struct string_array {
  size_t size;
  char *data[1];
};

struct string_array *read_file_hint(char const *fname, size_t expected_line_length);
struct string_array *read_file     (char const *fname);

#endif
