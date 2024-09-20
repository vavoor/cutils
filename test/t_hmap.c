#include "ut.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hmap.h"

void test_creation(void* pt)
{
  HMap map;

  HMap* mp = HMapCreate(&map, sizeof(int));
  UT_EXPECT(HMapLength(&map) == 0, "New hash map is empty");
}

void test_insertion(void* pt)
{
  HMap* map = HMapCreate(NULL, sizeof(int));

  int value;
  int previous;
  value = 0;
  int* i1 = HMapPut(map, "one", &value, NULL);
  UT_EXPECT(HMapLength(map) == 1, "Map has one element after insertion");
  UT_EXPECT(*i1 == value, "%d has been inserted", value);

  value = 1;
  int* i2 = HMapPut(map, "one", &value, &previous);
  UT_EXPECT(HMapLength(map) == 1, "Map has still one element after overwriting");
  UT_EXPECT(*i2 == value, "%d has been inserted", value);
  UT_EXPECT(i1 == i2, "Same key is mapped on the same element");

  int* slot = HMapFind(map, "one", &value);
  UT_EXPECT(slot != NULL, "one is found");
  UT_EXPECT(value == 1, "value is 1");

  slot = HMapFind(map, "two", NULL);
  UT_EXPECT(slot == NULL, "two is not in the map");

  HMapClear2(map);
  free(map);
}

void test_many_insertions(void* pt)
{
  char key[20];

  HMap* map = HMapCreate(NULL, sizeof(int));

  int* ret;
  int i;
  int N = 10000000;
  for (i = 0; i < N; i++) {
    sprintf(key, "k_%d", i);
    ret = HMapPutUnlessPresent(map, key, &i);
    UT_EXPECT(ret != NULL, "%s is inserted", key);
    UT_EXPECT(*ret == i, "%d has been inserted", i);
  }
  UT_EXPECT(HMapLength(map) == i, "Map has %d elements", N);

  HMapIt it;
  HMapPair pair;
  i = 0;
  for (HMapFirst(map, &it, &pair); !HMapEol(&it); HMapNext(&it, &pair)) {
    int* v = pair.element;
    UT_EXPECT(*v == i, "%d-th element is %d (is %d)", i, i, *ret);
    i++;
  }

  int value;
  for (i = 0; i < N; i++) {
    sprintf(key, "k_%d", i);
    ret = HMapFind(map, key, &value);
    UT_EXPECT(i == value, "key %s has value %d (is %d)", key, i, value);
    UT_EXPECT(value == *ret, "Element %d from has masp has been copied", i);
  }

  HMapClear2(map);
  free(map);
}
int main(int argc, const char* argv[])
{
  UT_start("Hash table test", _UT_FLAGS_NONE);
  UT_RUN(test_creation, NULL);
  UT_RUN(test_insertion, NULL);
  return UT_end() > 0;
}
