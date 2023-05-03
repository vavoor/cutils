#include "ut.h"
#include "llist.h"


static void test_creation(void* pt)
{
  LList list;
  LList* l = LListCreate(&list, sizeof(double));
  UT_expect(l == &list, "Passed list is returned");
  UT_expect(LListLength(&list) == 0, "New list is empty");
  UT_expect(LListElementSize(&list) == sizeof(double), "List can store doubles");

  LListClear(&list);
  UT_expect(LListLength(&list) == 0, "Cleared list is empty");
  UT_expect(LListElementSize(&list) == sizeof(double), "Cleared list still can store doubles");
}

static void test_appending(void* pt)
{
  LList* list = LListCreate(NULL, sizeof(double));

  double d;
  double* dp;

  d = 3.14;
  dp = (double*) LListAppend(list, &d);
  UT_expect(LListLength(list) == 1, "One element in list");
  UT_expect(*dp == d, "Element is in list");

  d = -2.71;
  dp = (double*) LListAppend(list, &d);
  UT_expect(LListLength(list) == 2, "Two element in list");
  UT_expect(*dp == d, "Element is in list");

  LListClear(list);
  UT_expect(LListLength(list) == 0, "Cleared list is empty");
}

static void test_iterating(void* pt)
{
  LList list;

  LListCreate(&list, sizeof(double));
  double d;
  double* dp;
  LListIt it;

  dp = LListFirst(&list, &it);
  UT_expect(dp == NULL, "First element of empty list is NULL");
  UT_expect(LListEol(&it), "First element of empty list of EOL");

  int i;
  const int N = 1000;
  for (i = 0; i < N; i++) {
    d = (double) i;
    LListAppend(&list, &d);
  }
  UT_expect(LListLength(&list) == N, "%d elements in list", N);

  i = 0;
  dp = LListFirst(&list, &it);
  UT_expect(*dp == (double) i, "First element is 0.0, but is %g", *dp);
  dp = LListGet(&it, &d);
  UT_expect(d == (double) i && *dp == d, "LListGet returns first element");

  while (!LListEol(&it)) {
    dp = LListGet(&it, &d);
    UT_expect(d == (double) i && *dp == d, "LListGet returns %d-th element", i);
    dp = LListNext(&it);
    i++;
    UT_expect((dp == NULL && i == N) || (*dp == (double) i), "LListGet returns element or NULL");
  }
  UT_expect(i == N, "Iterated over %d elements", N);

  LListClear(&list);
}

int main()
{
  UT_start("Linked List", _UT_FLAGS_NONE);

  UT_RUN1(test_creation);
  UT_RUN1(test_appending);
  UT_RUN1(test_iterating);

  return UT_end();
}
