#ifndef HEADER_a73c9785_73dd_4c9d_a7f4_bf47632c9749
#define HEADER_a73c9785_73dd_4c9d_a7f4_bf47632c9749

#define FU_FLAGS_NONE 0x00
#define FU_FLAGS_IGNORE_LARGE_FILES 0x01

/**
 * Read the contents of the file <fname> into memory as null-terminated
 * string. Memory is allocated and returned in <content>. It needs to be
 * deallocated by the client. At most, <max_size> bytes are read from the file.
 *
 * If FU_FLAGS_IGNORE_LARGE_FILES is set, files larger than <max_size> are
 * ignored and -1 is returned.
 *
 * If ...
 *   max_size < 0 : max_size is set to the default max size (1 MiB)
 *   max_size == 0 : the file size is not limited
 *
 * Returns strlen() of the file content. In case of error, -1 is returned
 * and <content> is set to NULL.
 */
int FUReadFile(const char* fname, int max_size, int flags, unsigned char** content);

/**
 * Write the escape character for <c> into <out> and return the number of
 * characters.
 */
int FUEscapeCChar(int c, unsigned char* out);
int FUEscapeJSONChar(int c, unsigned char* out);

/**
 * Write the escaped characters from <s> using <formatter and return the number
 * of characters written. <out> is null-terminaned. FUEscapeCChar or FUEscapeJSONChar
 * can be used as formatters.
 */
int FUEscapeStr(const unsigned char* s, int (*formatter)(int, char*), unsigned char* out);

/**
 * Decodes one codepoint following the UTF-8 convention. One to four bytes are read
 * from p. The codepoint is written to *codepoint.
 * Returns the number of bytes decoded or 0 in case of error.
 * 
 * Example:
 * 
 * unsigned char* p = utf8_encoded_buffer;
 * int codepoints[...];
 * int i = 0;
 * while (p < end_of_buffer) {
 *   p += FUUtf8Decode(p, &codepoints[i]);
 *   i++;
 * }
 */
int FUUtf8Decode(const unsigned char* p, int* codepoint);

int FUUtf8Encode(int codepoint, unsigned char* out);

#endif /* HEADER_a73c9785_73dd_4c9d_a7f4_bf47632c9749 */
