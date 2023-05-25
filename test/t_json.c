#include "ut.h"
#include "json.h"
#include <string.h>

static void single_string(void* pt)
{
  JSON j;
  char s[] = " \"foo bar\"\n";
  JSONParse(&j, s);
  UT_expect(j.type == J_STRING, "j is a string");
  UT_expect(strcmp(j.string.value, "foo bar") == 0, "j is foo bar");
}

static void single_object(void* pt)
{
  JSON j;
  char s[] = " { \"foo\" : \"bar\",\n \"baz\" : [ 123, 0.34, { \"xxx\":\n876 }, false, true ]  \n}\n";
  JSONParse(&j, s);
  JSONSerialize(&j, SerializeToFILE, stdout);
}

int main()
{
  UT_start("JSON", _UT_FLAGS_NONE);
  UT_RUN1(single_string);
  UT_RUN1(single_object);
  return UT_end();
}
