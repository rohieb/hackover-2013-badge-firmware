#include "read_file.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef READ_FILE_EXPECTED_LINE_LENGTH
#define READ_FILE_EXPECTED_LINE_LENGTH 25
#endif

/*
 * Der Plan ist, den Array-Kopf, den Index und die Daten direkt hintereinander in
 * einen großen Speicherblock zu legen, d.h.
 *
 * | size | data[0] | data[1] | data[2] | ... | "foo" | "bar" | "baz" | ... |
 * +------+---------+---------+---------+-----+-------+-------+-------+-----+
 *             |         |         |            ^       ^       ^
 *             |_________|_________|____________|       |       |
 *                       |_________|____________________|       |
 *                                 |____________________________|
 */

static int get_file_size(size_t *n, FILE *fd) {
  struct stat st;
  int err;

  err = fstat(fileno(fd), &st);
  if(err == 0) {
    *n = st.st_size;
  }

  return err;
}

static struct string_array *read_file_contents_raw(FILE *fd, size_t file_size, size_t expected_lines) {
  /* Ersten (geschätzten) Speicherbereich anfordern und den Dateiinhalt reindumpen. Noch keine
   * Zeilenverarbeitung; das kommt an anderer Stelle.
   */
  struct string_array *result;

  result = malloc(sizeof(struct string_array)           + /* Kopf   */
                  sizeof(char*) * (expected_lines - 1)  + /* Index  */
                  file_size + 1);                         /* Inhalt */

  if(result != NULL) {
    result->data[0] = (char*) (result->data + expected_lines);

    if(fread(result->data[0], 1, file_size, fd) != file_size) {
      free(result);
      result = NULL;
    } else {
      result->data[0][file_size] = '\0';
    }
  }

  return result;
}

static struct string_array *read_file_expand_index(struct string_array *partial,
                                                   char *next_string,
                                                   size_t file_size) {
  /* Wenn der erste, geschätzte Speicherbereich zu klein war (also mehr Zeilen in der Datei
   * bzw. die durchschnittliche Zeilenlänge größer war als erwartet), muss ausreichend Speicher
   * angefordert und der Inhalt umgeschichtet werden. Das passiert hier.
   *
   * Konvention: partial->size == Anzahl der bereits verarbeiteten Zahlen. Das ließe sich auch
   * aus dem Index zurückrechnen (((char**) data[0] - data) / sizeof(char*)), aber in der Haupt-
   * funktion kennen wir die Zahl schon, also spart das ein paar Zyklen.
   *
   * TODO: Vielleicht besser mit realloc?
   */
  struct string_array *result;
  char *p;
  size_t line_count;

  /* Zunächst muss gezählt werden, wie viele Zeilen die Datei tatsächlich hat; dementsprechend
   * wird später Speicher für den Index gebraucht.
   */
  line_count = partial->size;
  for(p = next_string; p; p = memchr(p + 1, '\n', partial->data[0] + file_size - p)) {
    *p = '\0';
    ++line_count;
  }

  /* Dann Speicher holen. */
  result = malloc(sizeof(struct string_array) + sizeof(char*) * (line_count - 1) + file_size + 1);

  if(result) {
    size_t i;

    /* Initialisierung. */
    result->data[0] = (char*) (result->data + line_count);
    result->size    = line_count;

    /* Daten rüberschaufeln. */
    memcpy(result->data[0], partial->data[0], file_size + 1);

    /* Alten Index übernehmen */
    for(i = 1; i < partial->size; ++i) {
      result->data[i] = result->data[0] + (partial->data[i] - partial->data[0]);
    }

    /* Und den restlichen Index ausrechnen und ranbappen. */
    for(; i < line_count; ++i) {
      result->data[i] = result->data[i - 1] + strlen(result->data[i - 1]) + 1;
    }
  }

  /* Der vorherige Speicherbereich wird nicht mehr gebraucht. */
  free(partial);
  return result;
}

struct string_array *read_file_hint(char const *fname, size_t expected_line_length) {
  /* Hier kommt alles zusammen. */
  struct string_array *result;
  FILE *fd;
  size_t file_size;
  size_t expected_lines;

  /* Dateiinhalt holen */
  fd = fopen(fname, "r");
  if(fd == NULL || get_file_size(&file_size, fd)) {
    return NULL;
  }

  expected_lines = file_size / expected_line_length + 1;
  result = read_file_contents_raw(fd, file_size, expected_lines);
  fclose(fd);

  if(result != NULL) {
    size_t i;
    char *p;

    /* In Zeilen aufspalten. */
    p = result->data[0];

    for(i = 1; (p = memchr(p + 1, '\n', result->data[0] + file_size - p)) && i < expected_lines; ++i) {
      *p = '\0';
      result->data[i] = p + 1;
    }

    result->size = i;

    /* Die Zeilen in der Datei waren kürzer als erwartet, also mehr als erwartet -> saurer Apfel. */
    if(p != NULL) {
      result = read_file_expand_index(result, p, file_size);
    }

    /* Eine leere letzte Zeile wird per Konvention nicht als Zeile aufgefasst. */
    if(result && *result->data[result->size - 1] == '\0') {
      --result->size;
    }
  }

  return result;
}

struct string_array *read_file(char const *fname) {
  return read_file_hint(fname, READ_FILE_EXPECTED_LINE_LENGTH);
}
