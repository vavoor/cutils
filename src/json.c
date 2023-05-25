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

void JSONObjectAddProperty(JSON* json, const char* name, JSON* value)
{
  assert(json->type == J_OBJECT);
  int overwritten;
  HMapPut2(&json->object.properties, name, value, &overwritten);
  if (overwritten) {
    // TODO error on duplicate property?
  }
}

void JSONClear(JSON* json)
{
  if (json != NULL) {
    int len;
    int i;

    switch (json->type) {
    case J_OBJECT:
      len = HMapLength(&json->object.properties);
      for (i = 0; i < len; i++) {
        JSON* j = HMapGetValue(&json->object.properties, i, NULL);
        JSONClear(j);
        HMapClear(&json->object.properties);
      }
      break;

    case J_ARRAY:
      len = AListLength(&json->array.elements);
      for (i = 0; i < len; i++) {
        JSON* j = AListGet(&json->array.elements, i, NULL);
        JSONClear(j);
        AListClear2(&json->array.elements);
      }
      break;

    case J_NUMBER:
      break;

    case J_STRING:
      free((void*)json->string.value);
      break;
    }
    json->type = J_NONE;
  }
}

void JSONArrayCreate(JSON* json)
{
  assert(json != NULL);

  json->array.type = J_ARRAY;
  AListCreate(&json->array.elements, sizeof(JSON), 0);
}

void JSONArrayAppend(JSON* json, JSON* element)
{
  assert(json->type == J_ARRAY);
  AListAppend(&json->array.elements, element);
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

void JSONBooleanCreate(JSON* json, int value)
{
  assert(json != NULL);
  json->boolean.type = J_BOOLEAN;
  json->boolean.value = value;
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

static int parse_boolean(JSON* json)
{
  if (strncasecmp(inp, "true", 4) == 0) {
    JSONBooleanCreate(json, 1);
    inp += 4;
    skip_ws();
    return 1;
  }
  else if (strncasecmp(inp, "false", 5) == 0) {
    JSONBooleanCreate(json, 0);
    inp += 5;
    skip_ws();
    return 1;
  }
  else {
    return 0;
  }
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
    return parse_boolean(value);
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

static void dump_value(JSON* json, void (*writer)(int c, void* pt), void* pt, int indent);

static void ind(int indent, void (*writer)(int c, void* pt), void* pt)
{
  int i;
  for (i = 0; i < indent; i++) {
    writer(' ', pt);
    writer(' ', pt);
  }
}

static void dump_chars(const char* s, void (*writer)(int c, void* pt), void* pt)
{
  const char* p = s;
  while (*p != '\0') {
    writer(*p, pt);
    p++;
  }
}

static void dump_number(JSON* json, void (*writer)(int c, void* pt), void* pt, int indent)
{
  char scratch[100];
  sprintf(scratch, "%g", json->number.value);
  dump_chars(scratch, writer, pt);
}

static void dump_string(JSON* json, void (*writer)(int c, void* pt), void* pt, int indent)
{
  writer('\"', pt);
  dump_chars(json->string.value, writer, pt);
  writer('\"', pt);
}

static void dump_boolean(JSON* json, void (*writer)(int c, void* pt), void* pt, int indent)
{
  if (json->boolean.value) {
    dump_chars("true", writer, pt);
  }
  else {
    dump_chars("false", writer, pt);
  }
}

static void dump_array(JSON* json, void (*writer)(int c, void* pt), void* pt, int indent)
{
  writer('[', pt);
  indent++;

  int len = AListLength(&json->array.elements);
  int i;
  for (i = 0; i < len; i++) {
    if (i > 0) {
      writer(',', pt);
    }
    writer('\n', pt);
    JSON* j = AListGet(&json->array.elements, i, NULL);
    ind(indent, writer, pt);
    dump_value(j, writer, pt, indent);
  }
  writer('\n', pt);
  indent--;
  ind(indent, writer, pt);
  writer(']', pt);
}

static void dump_object(JSON* json, void (*writer)(int c, void* pt), void* pt, int indent)
{
  writer('{', pt);
  indent++;

  int len = HMapLength(&json->object.properties);
  int i;
  for (i = 0; i < len; i++) {
    if (i > 0) {
      writer(',', pt);
    }
    writer('\n', pt);
    ind(indent, writer, pt);
    writer('\"', pt);
    dump_chars(HMapGetKey(&json->object.properties, i), writer, pt);
    writer('\"', pt);
    dump_chars(" : ", writer, pt);
    JSON* j = HMapGetValue(&json->object.properties, i, NULL);
    dump_value(j, writer, pt, indent);
  }
  writer('\n', pt);
  indent--;
  ind(indent, writer, pt);
  writer('}', pt);
}

static void dump_value(JSON* json, void (*writer)(int c, void* pt), void* pt, int indent)
{
  assert(json != NULL);

  switch (json->type) {
  case J_NUMBER:
    dump_number(json, writer, pt, indent);
    break;

  case J_STRING:
    dump_string(json, writer, pt, indent);
    break;

  case J_ARRAY:
    dump_array(json, writer, pt, indent);
    break;

  case J_OBJECT:
    dump_object(json, writer, pt, indent);
    break;

  case J_BOOLEAN:
    dump_boolean(json, writer, pt, indent);
    break;
  }
}

void JSONSerialize(JSON* json, void (*writer)(int c, void* pt), void* pt)
{
  dump_value(json, writer, pt, 0);
  writer('\n', pt);
}

void SerializeToFILE(int c, void* pt)
{
  FILE* out = (FILE*) pt;
  fputc(c, out);
}

void SerializeToString(int c, void* pt)
{
  char** s = (char**) pt;
  char* p = *s;
  *p++ = c;
  *p = '\0';
  *s = p;
}
