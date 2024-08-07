#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>

#include "ut.h"
#include "pset.h"
#include "pmap.h"

void test1(void* pt)
{
  char* FOO = "foo";
  char* BAR = "bar";
  
  Set set;
  SetNew(&set);
  UT_EXPECT(SetSize(&set) == 0, "Empty sets are empty");
  
  SetAdd(&set, FOO);
  UT_EXPECT(SetSize(&set) == 1, "Having added an element increases size");
  
  SetAdd(&set, BAR);
  UT_EXPECT(SetSize(&set) == 2, "Having added another element increases size");
  
  SetAdd(&set, FOO);
  UT_EXPECT(SetSize(&set) == 2, "Adding same element doesn´t increase size");
  
  SetAdd(&set, BAR);
  UT_EXPECT(SetSize(&set) == 2, "Adding same element doesn´t increase size");
  
  SetClear(&set);
  UT_EXPECT(SetSize(&set) == 0, "Cleared empty sets are empty");
}

void test2(void* pt)
{
  Set *set = SetNew(NULL);
  
  const int N = 10000;
  
  char* elements[N];
  
  int i;
  for (i = 0; i < N; i++) {
    asprintf(&elements[i], "K_%10.10d", i);
  }
  
  for (i = 0; i < N; i++) {
    int r = SetAdd(set, elements[i]);
    UT_EXPECT(r == 0, "Element not yet in the set");
  }
  
  for (i = N-1; i >= 0; i--) {
    int r = SetAdd(set, elements[i]);
    UT_EXPECT(r == 1, "Element not yet in the set");
  }
  
  for (i = 0; i < N; i++) {
    int r = SetContains(set, elements[i]);
    UT_EXPECT(r == 1, "Element is in set");
  }
  
  UT_EXPECT(SetContains(set, "blabla") == 0, "Element is not in set");
  
  SetClear(set);
  free(set);
}

int main()
{
  UT_start("set", _UT_FLAGS_NONE);
  UT_RUN(test1, NULL);
  UT_RUN(test2, NULL);
  return UT_end() > 0;
}
