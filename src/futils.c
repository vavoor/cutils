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

/*
 * Writes count bytes from buffer using method as-is.
 */
static void output_buffer_as_is(unsigned char* buffer, int count, int (*writer)(int c, void* pt), void* pt)
{
  int i;
  
  for (i = 0; i < count; i++) {
    writer(buffer[i], pt);
  }
}

/*
 * Writes count bytes from buffer using writer by encoding each byte in the UTF-8 encoding scheme.
 */
static void output_buffer_as_utf8(unsigned char* buffer, int count, int (*writer)(int c, void* pt), void* pt)
{
  int i;
  unsigned char out[10];
  
  for (i = 0; i < count; i++) {
    int n = FUUtf8Encode(buffer[i], out);
    int k;
    for (k = 0; k < n; k++) {
      writer(out[k], pt);
    }
  }
}

int FUUtf8Recode(int (*inp)(void* pt_in), void* pt_in, int (*outp)(int c, void* pt_out), void* pt_out)
{
  assert(inp != NULL);
  assert(outp != NULL);
  
  unsigned char buffer[10];
  int count = 0;
  int state = 1;
  int encoding = FU_ASCII7;
  
  int c;
  while ((c = inp(pt_in)) >= 0 ) {
  
    switch (state) {
      
    case 1: // nothing read
      buffer[count++] = c;
      if (0 <= c && c < 0x80) {
        state = 1;
        output_buffer_as_is(buffer, count, outp, pt_out);
        count = 0;
      }
      else if ((c & 0xE0) == 0xC0) {
        state = 2;
        encoding = encoding == FU_ASCII7 ? FU_UTF8 : encoding;
      }
      else if ((c & 0xF0) == 0xE0) {
        state = 3;
        encoding = encoding == FU_ASCII7 ? FU_UTF8 : encoding;
      }
      else if ((c & 0xF8) == 0xF0) {
        state = 5;
        encoding = encoding == FU_ASCII7 ? FU_UTF8 : encoding;
      }
      else {
        state = 1;
        encoding = FU_ASCII8;
        output_buffer_as_utf8(buffer, count, outp, pt_out);
        count = 0;
      }
      break;
      
    case 2: // UTF-8 header two byte codepoint read
      buffer[count++] = c;
      if ((0xC0 & c) == 0x80) {
        state = 1;
        output_buffer_as_is(buffer, count, outp, pt_out);
        count = 0;
      }
      else {
        state = 1;
        encoding = FU_ASCII8;
        output_buffer_as_utf8(buffer, count, outp, pt_out);
        count = 0;
      }
      break;
      
    case 3: // UTF-8 header three byte codepoint read
      buffer[count++] = c;
      if ((0xC0 & c) == 0x80) {
        state = 4;
      }
      else {
        state = 1;
        encoding = FU_ASCII8;
        output_buffer_as_utf8(buffer, count, outp, pt_out);
        count = 0;
      }
      break;
      
    case 4: // second byte of three byte codepoint read
      buffer[count++] = c;
      if ((0xC0 & c) == 0x80) {
        state = 1;
        output_buffer_as_is(buffer, count, outp, pt_out);
        count = 0;
      }
      else {
        state = 1;
        encoding = FU_ASCII8;
        output_buffer_as_utf8(buffer, count, outp, pt_out);
        count = 0;
      }
      break;
      
    case 5: // UTF-8 header of four byte codepoint read
      buffer[count++] = c;
      if ((0xC0 & c) == 0x80) {
        state = 6;
      }
      else {
        state = 1;
        encoding = FU_ASCII8;
        output_buffer_as_utf8(buffer, count, outp, pt_out);
        count = 0;
      }
      break;
      
    case 6: // second byte of four byte codepoint read
      buffer[count++] = c;
      if ((0xC0 & c) == 0x80) {
        state = 7;
      }
      else {
        state = 1;
        encoding = FU_ASCII8;
        output_buffer_as_utf8(buffer, count, outp, pt_out);
        count = 0;
      }
      break;
      
    case 7: // third byte of four byte codepoint read
      buffer[count++] = c;
      if ((0xC0 & c) == 0x80) {
        state = 1;
        output_buffer_as_is(buffer, count, outp, pt_out);
        count = 0;
      }
      else {
        state = 1;
        encoding = FU_ASCII8;
        output_buffer_as_utf8(buffer, count, outp, pt_out);
        count = 0;
      }
      break;
      
    default:
      assert("Illegal state encountered" == NULL);
      break;
    }
  }
  if (count > 0) {
    encoding = FU_ASCII8;
  }
  output_buffer_as_is(buffer, count, outp, pt_out);
  count = 0;
  
  return encoding;
}
