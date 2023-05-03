#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "template.h"

struct Pair {
  const char* key;
  const char* value;
};

struct Map {
  int count;
  struct Pair pairs[256];
};

static const char* key_finder(const char* key, void* pt)
{
  struct Map* map = (struct Map*) pt;
  int i;
  for (i = 0; i < map->count; i++) {
    if (strcmp(key, map->pairs[i].key) == 0) {
      return map->pairs[i].value;
    }
  }
  return NULL;
}

int template_print_filter_mapper(TEMPLATE_OUTPUT_FILTER filter, void* pt_filter, const char* template, TEMPLATE_KEY_MAPPER mapper, void* pt_mapper)
{
  char name[256];
  int n = 0;

  const char* p = template;
  while (*p != '\0') {
    if (p[0] == '%' && p[1] == '{') {
      p += 2;
      char* n = name;
      while (*p != '\0' && !(p[0] == '}' && p[1] == '%')) {
        *n++ = *p++;
      }
      *n = '\0';
      if (*p != '\0') {
        p += 2;
      }
      const char* value = mapper(name, pt_mapper);
      if (value != NULL) {
        const char* v = value;
        while (*v != '\0') {
          filter(*v, pt_filter);
          v++;
          n++;
        }
      }
    }
    else {
      filter(*p, pt_filter);
      p++;
      n++;
    }
  }

  return n;
}

/* Replaces %{NAME}% in template */
static int template_vprint_filter(TEMPLATE_OUTPUT_FILTER filter, void* pass_through, const char* template, va_list key_values)
{
  char* key;
  char* value;
  char name[256];
  struct Map map;

  map.count = 0;

  key = va_arg(key_values, char*);
  while (key != NULL) {
    value = va_arg(key_values, char*);
    assert(value != NULL);

    map.pairs[map.count].key = key;
    map.pairs[map.count].value = value;
    map.count++;
    assert(map.count < 256);

    key = va_arg(key_values, char*);
  }

  int n = template_print_filter_mapper(filter, pass_through, template, key_finder, &map);

  return n;
}

int template_print_filter(TEMPLATE_OUTPUT_FILTER filter, void* pass_through, const char* template, ...)
{
  va_list key_values;

  va_start(key_values, template);
  int n = template_vprint_filter(filter, pass_through, template, key_values);
  va_end(key_values);

  return n;
}

static void print_file_output_filter(int c, void* pass_through)
{
  FILE* out = (FILE*) pass_through;
  fputc(c, out);
}

int template_print_file(FILE* output, const char* template, ...)
{
  va_list key_values;

  va_start(key_values, template);
  int n = template_vprint_filter(print_file_output_filter, output, template, key_values);
  va_end(key_values);

  return n;
}
