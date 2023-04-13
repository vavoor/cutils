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
  HMapPut(map, "one", &value);
  UT_expect(HMapLength(map) == 1, "Map has one element after insertion");

  value = 1;
  HMapPut(map, "one", &value);
  UT_expect(HMapLength(map) == 1, "Map has still one element after overwriting");

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

  int i;
  int N = 1000000;
  for (i = 0; i < N; i++) {
    sprintf(key, "k_%d", i);
    int ret = HMapPut(map, key, &i);
    UT_expect(ret == 1, "%s is inserted", key);
  }
  UT_expect(HMapLength(map) == i, "Map has %d elements", N);

  for (i = 0; i < N; i++) {
    sprintf(key, "k_%d", i);
    int value;
    int slot = HMapFind(map, key, &value);
    UT_expect(slot != -1, "%s is valid key", key);
    UT_expect(value == i, "value of %s is %d not %d", key, i, value);
  }

  for (i = 0; i < N; i++) {
    sprintf(key, "k_%d", i);
    int ret = HMapPut(map, key, &i);
    UT_expect(ret == 0, "%s is overwritten", key);
  }
  UT_expect(HMapLength(map) == i, "Map has %d elements", N);

  for (i = N-1; i >= 0; i--) {
    sprintf(key, "k_%d", i);
    int value;
    int slot = HMapFind(map, key, &value);
    UT_expect(slot != -1, "%s is valid key", key);
    UT_expect(value == i, "value of %s is %d not %d", key, i, value);
  }

  int ret = HMapFind(map, "foo", NULL);
  UT_expect(ret == -1, "foo is not on the map");

  HMapClear(map);
  free(map);
}

int main()
{
  UT_start("Hash table test", _UT_FLAGS_NONE);
  UT_RUN(test_creation, NULL);
  UT_RUN(test_insertion, NULL);
  UT_RUN(test_many_insertions, NULL);
  return UT_end() > 0;
}
