#include "ut.h"
#include "json.h"
#include <string.h>

static void single_string(void* pt)
{
  JSON j;
  char* s = strdup(" \"foo bar\"\n");
  JSONParse(&j, s);
  UT_expect(j.type == J_STRING, "j is a string");
  UT_expect(strcmp(j.string.value, "foo bar") == 0, "j is foo bar");
  free(s);
}

static void single_object(void* pt)
{
  JSON j;
  char* s = strdup(" { \"foo\" : \"bar\",\n \"baz\" : [ 123, 0.34, {} ]  \n}\n");
  JSONParse(&j, s);
  JSONDump(&j, JSONToFILE, stdout);
  free(s);
}

int main()
{
  UT_start("JSON", _UT_FLAGS_NONE);
  UT_RUN1(single_string);
  UT_RUN1(single_object);
  return UT_end();
}
