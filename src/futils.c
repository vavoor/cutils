#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "futils.h"

int FUReadFile(const char* fname, char** content)
{
  assert(content != NULL);
  assert(fname != NULL);

  int size = -1;
  char* buffer = NULL;

  FILE* f = fopen(fname, "r");
  if (f != NULL) {
    fseek(f, 0, SEEK_END);
    int n = ftell(f);
    if (0 <= n && n < (1<<20)) {
      buffer = malloc(n + 1);
      assert(buffer != NULL);
      rewind(f);
      size = fread(buffer, 1, n, f);
      //~ if (size != n) {
        //~ fprintf(stderr, "%s: expecting %d bytes but could read only %d\n", fname, n, size);
      //~ }
      if (size <= 0) {
        free(buffer);
        buffer = NULL;
        size = -1;
      }
      else {
        buffer[size] = '\0';
      }
    }
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
