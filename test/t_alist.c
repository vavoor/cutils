#include "ut.h"
#include <stdlib.h>

#include "alist.h"

struct Element {
  int a;
  int b;
};

void new_alist_is_empty(void* pt)
{
  AList alist;

  AList* lp = AListCreate(&alist, sizeof(struct Element), 0);
  UT_EXPECT(lp == &alist, "AListCreate returns pointer to argument");
  UT_EXPECT(AListLength(&alist) == 0, "New AList has length 0");

  lp = AListCreate(NULL, sizeof(struct Element), 0);
  UT_EXPECT(AListLength(lp) == 0, "New AList (allocated) has length 0");
  AListClear2(lp);
  free(lp);
}

void appending(void* pt)
{
  AList al;
  struct Element e = {0,0};
  struct Element* ep;
  AListCreate(&al, sizeof(struct Element), 0);
  e.a = 1234;
  ep = AListAppend(&al, &e);
  UT_EXPECT(AListLength(&al) == 1, "Appending increases length by one");
  UT_EXPECT(ep->a == e.a, "Appended content is returned");

  e.a = 0;

  ep = AListGet(&al, 0, &e);
  UT_EXPECT(e.a == 1234, "AListGet copies previously inserted");
  UT_EXPECT(ep->a == 1234, "AListGet returns element previously inserted");

  AListClear2(&al);
}

void truncate(void* pt)
{
  AList al;
  struct Element e;
  int i;
  const int N = 10;

  AListCreate(&al, sizeof(struct Element), 0);
  for (i = 0; i < N; i++) {
    e.a = i;
    AListAppend(&al, &e);
  }
  AListTruncate(&al, 5);
  UT_EXPECT(AListLength(&al) == 5, "truncate reduces size");

  AListTruncate(&al, -2);
  UT_EXPECT(AListLength(&al) == 3, "reduced length by 2");

  AListTruncate(&al, 1);
  e.a = 99;
  AListAppend(&al, &e);
  UT_EXPECT(((struct Element*)AListGet(&al, 0, NULL))->a == 0, "expecting 0");
  UT_EXPECT(((struct Element*)AListGet(&al, -1, NULL))->a == 99, "expecting 99");

  AListClear2(&al);

}

void append_many(void* pt)
{
  AList al;
  struct Element e;
  int i;
  const int N = 1234567;

  AListCreate(&al, sizeof(struct Element), 0);
  for (i = 0; i < N; i++) {
    e.a = i;
    e.b = -i;
    AListAppend(&al, &e);
  }
  UT_EXPECT(AListLength(&al) == N, "length is %d", N);
  for (i = 0; i < N; i++) {
    struct Element* ep = AListGet(&al, i, NULL);
    UT_EXPECT(ep->a == i, "same value as inserted");
  }
  AListClear2(&al);
}

int main()
{
  UT_start("Array list tests", _UT_FLAGS_NONE);
  UT_RUN(new_alist_is_empty, NULL);
  UT_RUN(appending, NULL);
  UT_RUN(truncate, NULL);
  UT_RUN(append_many, NULL);
  return UT_end() > 0;
}
