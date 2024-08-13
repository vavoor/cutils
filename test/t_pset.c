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
  
  PSet set;
  PSetNew(&set);
  UT_EXPECT(PSetSize(&set) == 0, "Empty sets are empty");
  
  PSetAdd(&set, FOO);
  UT_EXPECT(PSetSize(&set) == 1, "Having added an element increases size");
  
  PSetAdd(&set, BAR);
  UT_EXPECT(PSetSize(&set) == 2, "Having added another element increases size");
  
  PSetAdd(&set, FOO);
  UT_EXPECT(PSetSize(&set) == 2, "Adding same element doesn´t increase size");
  
  PSetAdd(&set, BAR);
  UT_EXPECT(PSetSize(&set) == 2, "Adding same element doesn´t increase size");
  
  PSetClear(&set);
  UT_EXPECT(PSetSize(&set) == 0, "Cleared empty sets are empty");
}

void test2(void* pt)
{
  PSet *set = PSetNew(NULL);
  
  const int N = 10000;
  
  char* elements[N];
  
  int i;
  for (i = 0; i < N; i++) {
    asprintf(&elements[i], "K_%10.10d", i);
  }
  
  for (i = 0; i < N; i++) {
    int r = PSetAdd(set, elements[i]);
    UT_EXPECT(r == 0, "Element not yet in the set");
  }
  
  for (i = N-1; i >= 0; i--) {
    int r = PSetAdd(set, elements[i]);
    UT_EXPECT(r == 1, "Element not yet in the set");
  }
  
  for (i = 0; i < N; i++) {
    int r = PSetContains(set, elements[i]);
    UT_EXPECT(r == 1, "Element is in set");
  }
  
  UT_EXPECT(PSetContains(set, "blabla") == 0, "Element is not in set");
  
  PSetClear(set);
  free(set);
}

int main()
{
  UT_start("set", _UT_FLAGS_NONE);
  UT_RUN(test1, NULL);
  UT_RUN(test2, NULL);
  return UT_end() > 0;
}
