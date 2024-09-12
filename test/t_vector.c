#include "ut.h"
#include "vector.h"

#include <string.h>

void del_op(int i, void* element, void* pt)
{
  double d;

  memcpy(&d, element, sizeof(double));
  UT_EXPECT(-d == (int) i, "Del_op is called for each element (%d, %f)", i, d);
}

void test_creation(void* pt)
{
  Vector v;

  double d_in;
  double d_out;
  double* p_d;
  int i;
  //~ const int N = 256*256*256+1;
  //~ const int N = 16;
  //~ const int N = 255;
  const int N = 64*256;
  //~ const int N =257;

  VecCreate(&v, sizeof(double));
  UT_EXPECT(VecLength(&v) == 0, "New vector is empty");

  for (i = 0; i < N; i++) {
    d_in = (double) i;
    p_d = VecAppend(&v, &d_in);
    UT_EXPECT(*p_d == (double) i, "%d is appended", i);

    p_d = VecGet(&v, i, &d_out);
    UT_EXPECT(d_out == (double) i, "%d is at index %d", i, i);
    UT_EXPECT(*p_d == (double) i, "%d is at index %d", i, i);
  }

  for (i = N - 1; i >= 0; i--) {
    p_d = VecGet(&v, i, &d_out);
    UT_EXPECT(d_out == (double) i, "%d is at index %d", i, i);
    UT_EXPECT(*p_d == (double) i, "%d is at index %d", i, i);

    d_in = -i;
    p_d = VecSet(&v, i, &d_in);
    p_d = VecGet(&v, i, &d_out);
    UT_EXPECT(d_out == (double) -i, "%d is at index %d", -i, i);
    UT_EXPECT(*p_d == (double) -i, "%d is at index %d", -i, i);
  }

  VecClear(&v, del_op, NULL);
}

int main()
{
  UT_start("Testing vectors", _UT_FLAGS_NONE);

  UT_RUN1(test_creation);
  return UT_end() > 0;
}
