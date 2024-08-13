#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "futils.h"

int FUReadFile(const char* fname, int max_size, int flags, unsigned char** content)
{
  assert(content != NULL);
  assert(fname != NULL);

  max_size = max_size < 0 ? (1<<20) : max_size;

  int size = -1;
  unsigned char* buffer = NULL;

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

int FUEscapeCChar(int c, unsigned char* out)
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

int FUEscapeJSONChar(int c, unsigned char* out)
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

    //~ case '\'':
      //~ strcpy(out, "\\\'");
      //~ break;

    case '\"':
      strcpy(out, "\\\"");
      break;

    case '\\':
      strcpy(out, "\\\\");
      break;

    default:
      if ((0 <= c && c < ' ') || (127 <= c && c < 256)) {
        len = sprintf(out, "\\u%4.4X", c);
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

int FUEscapeStr(const unsigned char* s, int (*formatter)(int, char*), unsigned char* out)
{
  assert(s != NULL);
  assert(out != NULL);

  const unsigned char* in = s;
  unsigned char* o = out;

  while (*in != '\0') {
    o += formatter(*in, o);
    in++;
  }
  *o = '\0';
  return o - out;
}

int FUUtf8Decode(const unsigned char* p, int* cp)
{
  assert(p != NULL);
  assert(cp != NULL);
  
  if ((p[0] & 0x80) == 0) {
    *cp = p[0];
    return 1;
  }
  else if ((p[0] & 0xE0) == 0xC0) {
    if ((p[1] & 0x0C0) != 0x80) {
      return 0;
    }
    *cp = ((p[0] & 0x1F) << 6) | (p[1] & 0x3F);
    return 2;
  }
  else if ((p[0] & 0xF0) == 0xE0) {
    if ((p[1] & 0x0C0) != 0x80 || (p[2] & 0xC0) != 0x80) {
      return 0;
    }
    *cp = ((p[0] & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F);
    return 3;
  }
  else if ((p[0] & 0xF8) == 0xF0) {
    if ((p[1] & 0x0C0) != 0x80 || (p[2] & 0x0C0) != 0x80 || (p[3] & 0xC0) != 0x80) {
      return 0;
    }
    *cp = ((p[0] & 0x07) << 18) | ((p[1] & 0x3F) << 12) | ((p[2] & 0x3F) << 6) | (p[3] & 0x3F);
    return 4;
  }
  return 0;
}

int FUUtf8Encode(int cp, unsigned char* out)
{
  assert(out != NULL);
  
  unsigned char* p = out;
  
  if (cp < 0) {
    return 0;
  }
  
  if (cp < 0x0080) {
    p[0] = cp;
    return 1;
  }
  else if (cp < 0x0800) {
    p[0] = 0xC0 | ((cp >> 6) & 0x1F);
    p[1] = 0x80 | (cp & 0x3F);
    return 2;
  }
  else if (cp < 0x010000) {
    p[0] = 0xE0 | ((cp >> 12) & 0x0F);
    p[1] = 0x80 | ((cp >> 6) & 0x3F);
    p[2] = 0x80 | (cp & 0x3F);
    return 3;
  }
  else if (cp < 0x0110000) {
    p[0] = 0xF0 | ((cp >> 18) & 0x07);
    p[1] = 0x80 | ((cp >> 12) & 0x3F);
    p[2] = 0x80 | ((cp >> 6) & 0x3F);
    p[3] = 0x80 | (cp & 0x3F);
    return 4;
  }
  
  return 0;
}
