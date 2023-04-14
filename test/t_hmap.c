#include "ut.h"
#include <stdio.h>
#include <stdlib.h>

#include "hmap.h"

void test_creation(void* pt)
{
  HMap map;

  HMap* mp = HMapCreate(&map, sizeof(int));
  UT_expect(HMapLength(&map) == 0, "New hash map is empty");
}

void test_insertion(void* pt)
{
  HMap* map = HMapCreate(NULL, sizeof(int));

  int value;
  value = 0;
  int i1 = HMapPut(map, "one", &value);
  UT_expect(HMapLength(map) == 1, "Map has one element after insertion");

  value = 1;
  int i2 = HMapPut(map, "one", &value);
  UT_expect(HMapLength(map) == 1, "Map has still one element after overwriting");
  UT_expect(i1 == i2, "Same key is mapped on the same index");

  int slot;
  slot = HMapFind(map, "one", &value);
  UT_expect(slot == 0, "one is found");
  UT_expect(value == 1, "value is 1");

  slot = HMapFind(map, "two", NULL);
  UT_expect(slot == -1, "two is not in the map");

  HMapClear(map);
  free(map);
}

void test_many_insertions(void* pt)
{
  char key[20];

  HMap* map = HMapCreate(NULL, sizeof(int));

  int ret;
  int i;
  //~ int N = 1000000;
  int N = 10000;
  for (i = 0; i < N; i++) {
    sprintf(key, "k_%d", i);
    ret = HMapPutUnlessPresent(map, key, &i);
    UT_expect(ret >= 0, "%s is inserted", key);
  }
  UT_expect(HMapLength(map) == i, "Map has %d elements", N);

  ret = HMapPutUnlessPresent(map, key, &i);
  UT_expect(ret == -1, "%s is present already and cannot be inserted", key);

  for (i = 0; i < N; i++) {
    sprintf(key, "k_%d", i);
    int value;
    int slot = HMapFind(map, key, &value);
    UT_expect(slot != -1, "%s is valid key", key);
    UT_expect(value == i, "value of %s is %d not %d", key, i, value);
  }

  for (i = 0; i < N; i++) {
    int overwritten;
    int value = -i;
    sprintf(key, "k_%d", i);
    HMapPut2(map, key, &value, &overwritten);
    UT_expect(overwritten == 1, "%s is overwritten", key);
  }
  UT_expect(HMapLength(map) == i, "Map has %d elements", N);

  for (i = N-1; i >= 0; i--) {
    sprintf(key, "k_%d", i);
    int value;
    int slot = HMapFind(map, key, &value);
    UT_expect(slot != -1, "%s is valid key", key);
    UT_expect(value == -i, "value of %s is %d not %d", key, -i, value);
  }

  ret = HMapFind(map, "foo", NULL);
  UT_expect(ret == -1, "foo is not on the map");

  HMapClear(map);
  free(map);
}

void test_symbolmap(void* pt)
{
  HMap map;
  HMapCreate(&map, 0);

  const char foo[] = "foo";
  const char bar[] = "bar";
  int i1a, i2a, i1b, i2b;

  i1a = HMapPut(&map, foo, NULL);
  i2a = HMapPut(&map, bar, NULL);
  i1b = HMapPut(&map, foo, NULL);
  i2b = HMapPut(&map, bar, NULL);

  UT_expect(i1a == i1b, "Same key (foo), same index");
  UT_expect(i2a == i2b, "Same key (bar), same index");

  UT_expect(strcmp(foo, HMapGetKey(&map, i1a)) == 0, "foo is in the table");
  UT_expect(strcmp(bar, HMapGetKey(&map, i2a)) == 0, "bar is in the table");

  HMapClear(&map);
}

int main()
{
  UT_start("Hash table test", _UT_FLAGS_NONE);
  UT_RUN(test_creation, NULL);
  UT_RUN(test_insertion, NULL);
  UT_RUN(test_many_insertions, NULL);
  UT_RUN(test_symbolmap, NULL);
  return UT_end() > 0;
}
