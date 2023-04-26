#ifndef HEADER_a73c9785_73dd_4c9d_a7f4_bf47632c9749
#define HEADER_a73c9785_73dd_4c9d_a7f4_bf47632c9749

/**
 * Read the contents of the file <fname> into memory as null-terminated
 * string. Memory is allocated and returned in <content>. It needs to be
 * deallocated by the client.
 *
 * Returns strlen() of the file content. In case of error, -1 is returned
 * and <content> is set to NULL.
 */
int FUReadFile(const char* fname, char** content);

/**
 * Write the escape character for <c> into <out> and return the number of
 * characters.
 */
int FUEscapeChar(int c, char* out);

#endif /* HEADER_a73c9785_73dd_4c9d_a7f4_bf47632c9749 */
