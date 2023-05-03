#ifndef HEADER_b18487a0_4508_489a_981a_80e9ee1860d7
#define HEADER_b18487a0_4508_489a_981a_80e9ee1860d7

#include <stdio.h>

typedef void (*TEMPLATE_OUTPUT_FILTER)(int c, void* pass_through);
typedef const char* (*TEMPLATE_KEY_MAPPER)(const char* key, void* pass_through);

/**
 * Replace the variables in <template> with their value and write the result
 * into the file <output>.
 *
 * A variable in the template is a string - the key - enclosed in %{ and }%.
 * The list of keys and values is passed as argument list and is terminated with
 * a NULL. Undefined variables are replaced with the empty string.
 *
 * Returns the number of characters written to <output>.
 *
 * Example:
 *
 * char templ[] = "The %{color}% %{the animal}% is %{color}%ish\n";
 * template_print_file(stdout, templ, "the animal", "fox", "color", "brown", NULL);
 *
 * produces "The brown fox is brownish\n"
 */
int template_print_file(FILE* output, const char* template, ... /* key, value, ..., NULL */);

/**
 * Same as template_print_file except that the output is written by calling
 * the output filter function <filter> for each character. <pass_through> is
 * passed to <filter> without any modifications.
 *
 * Example:
 *
 * void file_filter(int c, void* pt)
 * {
 *   FILE* output = (FILE*) pt;
 *   fputc(c, output);
 * }
 *
 * template_print_filter(file_filter, stdout, "Foo %{bar}%", "bar", "baz", NULL);
 *
 * is the same as
 *
 * template_print_file(stdout, "Foo %{bar}%", "bar", "baz", NULL);
 */
int template_print_filter(TEMPLATE_OUTPUT_FILTER filter, void* pass_through, const char* template, ... /* key, value, ..., NULL */);

/**
 * Same as template_print_filter except that the mapping of keys on values is
 * done by calling the function <mapper>.
 *
 * Example:
 *
 * void file_filter(int c, void* pt)
 * {
 *   FILE* output = (FILE*) pt;
 *   fputc(c, output);
 * }
 *
 * const char* fox_mapper(const char* key, void* pt)
 * {
 *   const char* default = (const char*) pt;
 *   if (strcmp(key, "animal") == 0) {
 *     return "Fox";
 *   }
 *   else {
 *     return default;
 *   }
 * }
 *
 * template_print_filter_mapper(file_filter, stdout, "%{animal}% is %{bar}%", fox_mapper, "whatever");
 *
 * writes "Fox is whatever" to stdout.
 */
int template_print_filter_mapper(TEMPLATE_OUTPUT_FILTER filter, void* pt_filter, const char* template,
                                  TEMPLATE_KEY_MAPPER mapper, void* pt_mapper);

#endif /* HEADER_b18487a0_4508_489a_981a_80e9ee1860d7 */
