#ifndef HEADER_f59a2dbf_ee40_47e2_a6b5_8b64df707acf
#define HEADER_f59a2dbf_ee40_47e2_a6b5_8b64df707acf

#include "alist.h"
#include "hmap.h"

enum { J_NONE, J_OBJECT, J_ARRAY, J_NUMBER, J_STRING, J_BOOLEAN };
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

  struct JSONBoolean {
    int type;
    int value;
  } boolean;
} JSON;

void JSONObjectCreate(JSON* json);
void JSONObjectAddProperty(JSON* json, const char* name, JSON* value);
void JSONArrayCreate(JSON* json);
void JSONArrayAppend(JSON* json, JSON* element);
void JSONStringCreate(JSON* json, const char* value);
void JSONNumberCreate(JSON* json, double value);
void JSONBooleanCreate(JSON* json, int value);

/**
 * Returns true if parsed successfully
 */
int JSONParse(JSON* json, const char* json_string);

void JSONSerialize(JSON* json, void (*writer)(int c, void* pt), void* pt);
void SerializeToFILE(int c, void* pt /* FILE* */);
void SerializeToString(int c, void* pt /* char** */);

#endif /* HEADER_f59a2dbf_ee40_47e2_a6b5_8b64df707acf */
