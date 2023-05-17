#ifndef HEADER_f59a2dbf_ee40_47e2_a6b5_8b64df707acf
#define HEADER_f59a2dbf_ee40_47e2_a6b5_8b64df707acf

#include "alist.h"
#include "hmap.h"

enum { J_NONE, J_OBJECT, J_ARRAY, J_NUMBER, J_STRING };
typedef union JSON {
  int type;

  struct JSONObject {
    int type;
    HMap properties;
  } object;

  struct JSONArray {
    int type;
    AList elements;
  } array;

  struct JSONNumber {
    int type;
    double value;
  } number;

  struct JSONString {
    int type;
    const char* value;
  } string;
} JSON;

void JSONObjectCreate(JSON* json);
void JSONArrayCreate(JSON* json);
void JSONStringCreate(JSON* json, const char* value);
void JSONNumberCreate(JSON* json, double value);

/**
 * Returns true if parsed successfully
 */
int JSONParse(JSON* json, const char* json_string);
void JSONDump(JSON* json, void (*dump)(int c, void* pt), void* pt);
void JSONToFILE(int c, void* pt);
void JSONToString(int c, void* pt /* char** */);

#endif /* HEADER_f59a2dbf_ee40_47e2_a6b5_8b64df707acf */
