#ifndef HEADER_a73c9785_73dd_4c9d_a7f4_bf47632c9749
#define HEADER_a73c9785_73dd_4c9d_a7f4_bf47632c9749

#define FU_FLAGS_NONE 0x00
#define FU_FLAGS_IGNORE_LARGE_FILES 0x01

/**
 * Read the contents of the file <fname> into memory as null-terminated
 * string. Memory is allocated and returned in <content>. It needs to be
 * deallocated by the client.
 *
 * max_size < 0 : default max size (1 MiB)
 * max_size == 0 : no limit
 * max_size > 0 : specified limit
 *
 * Returns strlen() of the file content. In case of error, -1 is returned
 * and <content> is set to NULL.
 */
int FUReadFile(const char* fname, int max_size, int flags, char** content);

/**
 * Write the escape character for <c> into <out> and return the number of
 * characters.
 */
int FUEscapeChar(int c, char* out);

#endif /* HEADER_a73c9785_73dd_4c9d_a7f4_bf47632c9749 */
