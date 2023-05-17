#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "json.h"

static const char* inp;

static void skip_ws(void)
{
  while (isspace(*inp)) {
    inp++;
  }
}

static void skip_string(void)
{
  const char* p = inp;

  if (*p == '\'' || *p == '\"') {
    int sep = *p++;
    while (*p != '\0' && *p != sep) {
      if (p[0] == '\\' && p[1] != '\0') {
        p++;
      }
      p++;
    }
    if (*p == sep) {
      p++;
      inp = p;
    }
  }
}

static void consume(void)
{
  inp++;
  skip_ws();
}

void JSONObjectCreate(JSON* json)
{
  assert(json != NULL);

  json->object.type = J_OBJECT;
  HMapCreate(&json->object.properties, sizeof(JSON));
}

void JSONClear(JSON* json)
{
  // TODO
  json->type = J_NONE;
}

void JSONArrayCreate(JSON* json)
{
  assert(json != NULL);

  json->array.type = J_ARRAY;
  AListCreate(&json->array.elements, sizeof(JSON), 0);
}

void JSONStringCreate(JSON* json, const char* value)
{
  assert(json != NULL);

  json->string.type = J_STRING;
  json->string.value = strdup(value);
}

void JSONNumberCreate(JSON* json, double value)
{
  assert(json != NULL);

  json->number.type = J_NUMBER;
  json->number.value = value;
}


static int parse_object(JSON*);
static int parse_array(JSON*);

static int parse_string(JSON* json)
{
  const char* value = inp;
  skip_string();
  if (value == inp) {
    return 0;
  }
  /* remove delimiters */
  value = &value[1];
  char* to = (char*) &inp[-1];
  int last = *to;
  *to = '\0';
  JSONStringCreate(json, value);
  *to = last;
  skip_ws();
  return 1;
}

static int parse_number(JSON* json)
{
  const char* value = inp;
  while (isdigit(*inp) || *inp == '.') {
    inp++;
  }
  if (value == inp) {
    return 0;
  }
  char* to = (char*) inp;
  int last = *to;
  *to = '\0';
  double v = atof(value);
  *to = last;

  JSONNumberCreate(json, v);
  skip_ws();
  return 1;
}

static int parse_value(JSON* value)
{
  if (*inp == '{') {
    return parse_object(value);
  }
  else if (*inp == '[') {
    return parse_array(value);
  }
  else if (*inp == '\'' || *inp == '\"') {
    return parse_string(value);
  }
  else if (isdigit(*inp)) {
    return parse_number(value);
  }
  else {
    return 0;
  }
}

/*
 * array -> '[' (value ( ',' value )* )? ']'
 * Return true on success
 */
static int parse_array(JSON* json)
{
  JSONArrayCreate(json);

  if (*inp != '[') {
    return 0;
  }
  consume();

  if (*inp != ']') {
    JSON value;
    if (!parse_value(&value)) {
      return 0;
    }
    AListAppend(&json->array.elements, &value);
    while (*inp == ',') {
      consume();
      if (!parse_value(&value)) {
        return 0;
      }
      AListAppend(&json->array.elements, &value);
    }
  }

  if (*inp != ']') {
    return 0;
  }
  consume();
  return 1;
}

/*
 * Adds a property to <jobj>.
 * Returns true on success.
 *
 * property -> STR ':' value
 */
static int parse_property(JSON* jobj)
{
  assert(jobj->type == J_OBJECT);

  const char* key = inp;
  skip_string();
  if (key == inp) {
    return 0;
  }

  /* remove string delimiters */
  key = &key[1];
  char* to = (char*) &inp[-1];

  skip_ws();

  if (*inp != ':') {
    return 0;
  }
  consume();

  JSON value;
  if (!parse_value(&value)) {
    return 0;
  }

  int last = *to;
  *to = '\0';

  int overwritten;
  HMapPut2(&jobj->object.properties, key, &value, &overwritten);
  *to = last;

  if (overwritten) {
    // TODO : error on duplicate property??
  }

  return 1;
}

/*
 * obj -> '{' ( property ( ',' property )* )? '}'
 * Returns true on success
 */
static int parse_object(JSON* json)
{
  JSONObjectCreate(json);

  if (*inp != '{') {
    return 0;
  }

  consume();

  if (*inp != '}') {
    if (!parse_property(json)) {
      return 0;
    }

    while (*inp == ',') {
      consume();
      if (!parse_property(json)) {
        return 0;
      }
    }
  }

  if (*inp != '}') {
    return 0;
  }

  consume();
  return 1;
}

int JSONParse(JSON* json, const char* json_string)
{
  assert(json != NULL);
  assert(json_string != NULL);

  inp = json_string;
  skip_ws();

  int success = parse_value(json);

  if (success && *inp == '\0') {
    return 1;
  }
  else {
    JSONClear(json);
    return 0;
  }
}

static void dump_value(JSON* json, void (*dump)(int c, void* pt), void* pt);

static void dump_chars(const char* s, void (*dump)(int c, void* pt), void* pt)
{
  const char* p = s;
  while (*p != '\0') {
    dump(*p, pt);
    p++;
  }
}

static void dump_number(JSON* json, void (*dump)(int c, void* pt), void* pt)
{
  assert(json->type == J_NUMBER);
  char scratch[100];
  sprintf(scratch, "%g", json->number.value);
  dump_chars(scratch, dump, pt);
}

static void dump_string(JSON* json, void (*dump)(int c, void* pt), void* pt)
{
  dump('\"', pt);
  dump_chars(json->string.value, dump, pt);
  dump('\"', pt);
}

static void dump_array(JSON* json, void (*dump)(int c, void* pt), void* pt)
{
  dump_chars("[ ", dump, pt);
  int len = AListLength(&json->array.elements);
  int i;
  for (i = 0; i < len; i++) {
    if (i > 0) {
      dump_chars(", ", dump, pt);
    }
    JSON* j = AListGet(&json->array.elements, i, NULL);
    dump_value(j, dump, pt);
  }
  dump_chars(" ]", dump, pt);
}

static void dump_object(JSON* json, void (*dump)(int c, void* pt), void* pt)
{
  dump_chars("{ ", dump, pt);

  int len = HMapLength(&json->object.properties);
  int i;
  for (i = 0; i < len; i++) {
    if (i > 0) {
      dump_chars(", ", dump, pt);
    }
    dump('\"', pt);
    dump_chars(HMapGetKey(&json->object.properties, i), dump, pt);
    dump('\"', pt);
    dump_chars(" : ", dump, pt);
    JSON* j = HMapGetValue(&json->object.properties, i, NULL);
    dump_value(j, dump, pt);
  }
  dump_chars(" }", dump, pt);
}

static void dump_value(JSON* json, void (*dump)(int c, void* pt), void* pt)
{
  assert(json != NULL);

  switch (json->type) {
  case J_NUMBER:
    dump_number(json, dump, pt);
    break;

  case J_STRING:
    dump_string(json, dump, pt);
    break;

  case J_ARRAY:
    dump_array(json, dump, pt);
    break;

  case J_OBJECT:
    dump_object(json, dump, pt);
    break;
  }
}

void JSONDump(JSON* json, void (*dump)(int c, void* pt), void* pt)
{
  dump_value(json, dump, pt);
  dump('\n', pt);
}

void JSONToFILE(int c, void* pt)
{
  FILE* out = (FILE*) pt;
  fputc(c, out);
}

void JSONToString(int c, void* pt)
{
  char** s = (char**) pt;
  char* p = *s;
  *p++ = c;
  *p = '\0';
  *s = p;
}
