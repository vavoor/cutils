#include "ut.h"
#include <stdlib.h>

#include "vector.h"

void test_create_and_clear(void* pt)
{
  Vector v;
  VecCreate(&v);
  UT_EXPECT(VecLength(&v) == 0, "New vector has length 0");
  VecClear2(&v);
}

void test_appends(void* pt)
{
  Vector v;
  VecCreate(&v);
  char* s = "foobar";
  VecAppend(&v, s);
  UT_EXPECT(VecLength(&v) == 1, "Length is one after inserting an element");
  UT_EXPECT(VecGet(&v, 0) == s, "Read and write");
  VecClear2(&v);
}

void test_many(void* pt)
{
  union V {
    void* p;
    int i;
  } val, x;
  
  const int N = 256*256*256+1;
  Vector v;
  VecCreate(&v);
  int i;
  for (i = 0; i < N; i++) {
    val.i = i;
    VecAppend(&v, val.p);
  }
  UT_EXPECT(VecLength(&v) == i, "Long after many insertions");
  
  for (i = 0; i < N; i++) {
    val.p = VecGet(&v, i);
    UT_EXPECT(val.i == i, "Reading the same value (%d,%d)", i, val.i);
  }
  
  for (i = N-1; i>=0; i--) {
    val.i = -i;
    x.p = VecSet(&v, i, val.p);
    UT_EXPECT(x.i == i, "Still the same value");
    x.p = VecGet(&v, i);
    UT_EXPECT(x.i == -i, "Value negated");
  }
  
  VecTruncate2(&v, 12);
  UT_EXPECT(VecLength(&v) == 12, "Truncating to 12");
  for (i = 11; i >= 0; i--) {
    x.p = VecGet(&v, i);
    UT_EXPECT(x.i == -i, "Value negated again");
  }
  x.p = VecGet(&v, 12);
  UT_EXPECT(x.i == 0, "NULL pointer returned");
  VecClear2(&v);
}

#define TEST_CLEAR_N 257
void del_op(int i, void* el, void* pt)
{
  union V {
    void* p;
    int i;
  } val;
  int* call_count = pt;
  
  val.p = el;
  UT_EXPECT(val.i == i, "del_op is called with right content (index=%d, content=%d)", i, val.i);
  *call_count += 1;
}

int test_clear_call_count;
void test_clear(void* pt)
{
  union V {
    void* p;
    int i;
  } val;
  
  Vector v;
  
  test_clear_call_count = 0;
  UT_EXPECT(VecCreate(&v) == &v, "VecCreate returns address of vector");
  int i;
  for (i = 0; i < TEST_CLEAR_N; i++) {
    val.i = i;
    VecAppend(&v, val.p);
  }
  
  VecClear(&v, del_op, &test_clear_call_count);
  UT_EXPECT(test_clear_call_count == TEST_CLEAR_N, "del_op is called for each element");
}


int main(int argc, const char* argv[])
{
  UT_start("Vector tests", _UT_FLAGS_NONE);
  UT_RUN(test_create_and_clear, NULL);
  //~ UT_RUN(test_appends, NULL);
  UT_RUN(test_many, NULL);
  UT_RUN(test_clear, NULL);
  return UT_end() > 0;
}
