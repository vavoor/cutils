#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "futils.h"

int FUReadFile(const char* fname, int max_size, int flags, char** content)
{
  assert(content != NULL);
  assert(fname != NULL);

  max_size = max_size < 0 ? (1<<20) : max_size;

  int size = -1;
  char* buffer = NULL;

  FILE* f = fopen(fname, "r");
  if (f == NULL) {
    goto exit;
  }

  fseek(f, 0, SEEK_END);
  int n = ftell(f);

  if (n < 0) {
    goto exit;
  }

  if ((flags & FU_FLAGS_IGNORE_LARGE_FILES) && (max_size > 0) && (n > max_size)) {
    goto exit;
  }

  rewind(f);

  int to_read = (max_size > 0) && (n > max_size) ? max_size : n;
  buffer = malloc(to_read + 1);
  assert(buffer != NULL);

  size = fread(buffer, 1, to_read, f);
  if (size < 0) {
    free(buffer);
    buffer = NULL;
    size = -1;
    goto exit;
  }

  buffer[size] = '\0';

exit:
  if (f != NULL) {
    fclose(f);
  }
  *content = buffer;
  return size;
}

int FUEscapeChar(int c, char* out)
{
  assert(0 <= c && c < 256);
  int len = 2;

  switch (c) {
    case '\b':
      strcpy(out, "\\b");
      break;

    case '\n':
      strcpy(out, "\\n");
      break;

    case '\t':
      strcpy(out, "\\t");
      break;

    case '\f':
      strcpy(out, "\\f");
      break;

    case '\r':
      strcpy(out, "\\r");
      break;

    case '\'':
      strcpy(out, "\\\'");
      break;

    case '\"':
      strcpy(out, "\\\"");
      break;

    case '\\':
      strcpy(out, "\\\\");
      break;

    default:
      if ((0 <= c && c < ' ') || (128 <= c && c < 256)) {
        len = sprintf(out, "\\x%2.2X", c);
      }
      else {
        *out++ = c;
        *out = '\0';
        len = 1;
      }
      break;
  }

  return len;
}
