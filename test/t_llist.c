#include "ut.h"
#include "llist.h"
#include <stdlib.h>


static void test_creation(void* pt)
{
  LList list;
  LList* l = LListCreate(&list, sizeof(double));
  UT_EXPECT(l == &list, "Passed list is returned");
  UT_EXPECT(LListLength(&list) == 0, "New list is empty");
  UT_EXPECT(LListElementSize(&list) == sizeof(double), "List can store doubles");

  LListClear2(&list);
  UT_EXPECT(LListLength(&list) == 0, "Cleared list is empty");
  UT_EXPECT(LListElementSize(&list) == sizeof(double), "Cleared list still can store doubles");
}

static void test_appending(void* pt)
{
  LList* list = LListCreate(NULL, sizeof(double));

  double d;
  double* dp;

  d = 3.14;
  dp = (double*) LListAppend(list, &d);
  UT_EXPECT(LListLength(list) == 1, "One element in list");
  UT_EXPECT(*dp == d, "Element is in list");

  d = -2.71;
  dp = (double*) LListAppend(list, &d);
  UT_EXPECT(LListLength(list) == 2, "Two element in list");
  UT_EXPECT(*dp == d, "Element is in list");

  LListClear2(list);
  UT_EXPECT(LListLength(list) == 0, "Cleared list is empty");
}

static void test_iterating(void* pt)
{
  LList list;

  LListCreate(&list, sizeof(double));
  double d;
  double* dp;
  LListIt it;

  dp = LListFirst(&list, &it);
  UT_EXPECT(dp == NULL, "First element of empty list is NULL");
  UT_EXPECT(LListEol(&it), "First element of empty list of EOL");

  int i;
  const int N = 1000;
  for (i = 0; i < N; i++) {
    d = (double) i;
    LListAppend(&list, &d);
  }
  UT_EXPECT(LListLength(&list) == N, "%d elements in list", N);

  i = 0;
  dp = LListFirst(&list, &it);
  UT_EXPECT(*dp == (double) i, "First element is 0.0, but is %g", *dp);
  dp = LListData(&it, &d);
  UT_EXPECT(d == (double) i && *dp == d, "LListData returns first element");

  while (!LListEol(&it)) {
    dp = LListData(&it, &d);
    UT_EXPECT(d == (double) i && *dp == d, "LListData returns %d-th element", i);
    dp = LListNext(&it);
    i++;
    UT_EXPECT((dp == NULL && i == N) || (*dp == (double) i), "LListData returns element or NULL");
  }
  UT_EXPECT(i == N, "Iterated over %d elements", N);

  LListClear2(&list);
}

static void test_iteration_pattern1(void* pt)
{
  LList* list = LListCreate(NULL, sizeof(double));
  LListClear2(list);

  double d;
  int i;
  for (i = 0; i < 10; i++) {
    d = (double) -i;
    LListAppend(list, &d);
  }

  LListIt it;
  i = 0;
  for (LListFirst(list, &it); !LListEol(&it); LListNext(&it)) {
    double el;
    LListData(&it, &el);
    UT_EXPECT(el == (double) -i, "%d expected", -i);
    i++;
  }

  LListClear2(list);
  free(list);
}

static void test_iteration_pattern2(void* pt)
{
  LList* list = LListCreate(NULL, sizeof(double));
  LListClear2(list);

  double d;
  int i;
  for (i = 0; i < 10; i++) {
    d = (double) -i;
    LListAppend(list, &d);
  }

  LListIt it;
  i = 0;
  double* ep = LListFirst(list, &it);
  UT_EXPECT(*ep == (double) -i, "%d expected", -i);
  i++;
  while (ep != NULL) {
    ep = LListNext(&it);
    UT_EXPECT(ep == NULL || *ep == (double) -i, "%d expected", -i);
    i++;
  }

  LListClear2(list);
  free(list);
}

static int add(int i, void* element, void* pt)
{
  double* e = (double*) element;
  double* sum = (double*) pt;

  *sum += *e;

  return 0;
}

static void test_for_all(void* pt)
{
  LList list;
  LListCreate(&list, sizeof(double));

  const int N =100;
  int i;

  double sum = 0.0;
  double value;
  for (i = 0; i < N; i++) {
    value = (double) i;
    sum += value;
    LListAppend(&list, &value);
  }

  value = 0.0;
  LListForAll(&list, add, &value);
  UT_EXPECT(value == sum, "Sum of elements is %g", sum);
}

int main()
{
  UT_start("Linked List", _UT_FLAGS_NONE);

  UT_RUN1(test_creation);
  UT_RUN1(test_appending);
  UT_RUN1(test_iterating);
  UT_RUN1(test_iteration_pattern1);
  UT_RUN1(test_iteration_pattern2);
  UT_RUN1(test_for_all);

  return UT_end();
}
