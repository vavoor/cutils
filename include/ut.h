/*
 * UT (Unit Test) is a very simple unit test framework for the C programming
 * language.
 *
 * Example
 * -------
 *
 * #include "ut.h"
 *
 * void test1(void* pass_through) {
 *   UT_FAIL("This test always fails");
 * }
 *
 * void test_compare_string(void* pass_through) {
 *   int* ip = (int*) pass_through;
 *   UT_EXPECT(*ip < 42, "parameter is less than 42, but it's %d", *ip);
 * }
 *
 * int main() {
 *   UT_start("Demo", _UT_FLAGS_VERBOSE);
 *   UT_run("Always fail", test1, NULL);
 *   int parameter = 43;
 *   UT_RUN(test_compare_string, &parameter);
 *   return UT_end() > 0;
 * }
 *
 *
 * How to use UT
 * -------------
 *
 * All code is in this one header file. All variables and functions are declared
 * as static, i.e. it can be included multiple times into the same executable.
 *
 * Test suites are enclosed in calls to ut_start and ut_end:
 *
 * void ut_start(const char* test_suite_name, int flags)
 *   test_suite_name is
 *   flags is an OR-ed bitmask consting of
 *   _UT_FLAGS_VERBOSE List all tests that are executed. By default only failing tests are reported.
 *   _UT_FLAGS_ABORT   Abort when a test fails. This can be caught in the debugger.
 *   _UT_FLAGS_NONE    should be used to indicate that no flag is set
 *
 * int ut_end(void)
 *   returns the number of failing tests since the last call to ut_start.
 *
 * A test is expressed as a function with the this signature:
 *
 * void test_function(void* pass_through)
 *   pass_through is a pointer to arbitrary data that can be used to
 *   parameterize the test.
 *
 * A test function is called with UT_run or using the macro UT_RUN:
 *
 * void UT_run(const char* name, void (*test)(void*), void* pass_through)
 *   name is the name of the test used for reporting
 *   test is the pointer to the test function
 *   pass_through is a pointer to arbitrary data that is passed to the test function
 *
 * The macro UT_RUN(test_function, pass_though) calls the test function through UT_run
 * and used the name of the test function also as test name. I.e. UT_RUN(x, y) is the same
 * as UT_run("x", x, y).
 *
 * The macro UT_RUN1(test_function) is the same as UT_RUN except that pass_through
 * is set to NULL.
 *
 * A test function reports test results with UT_expect or UT_fail:
 *
 * void UT_fail(const char* msg, ...)
 *   Marks the test as failed and prints out msg. msg is a format string as for printf.
 *
 * void UT_expect(int condition, const char *msg, ...)
 *   Marks the test as failed if condition is false. In case of failure, msg
 *   is printed out. msg is a format string as for printf.
 */

#ifndef HEADER_22bf4689_c2e1_4622_9adb_07746882f399
#define HEADER_22bf4689_c2e1_4622_9adb_07746882f399

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define _UT_INIT_MARKER 0x22bf4689
#define _UT_FLAGS_NONE 0x00000000
#define _UT_FLAGS_VERBOSE 0x00000001
#define _UT_FLAGS_ABORT 0x00000002

static struct {
  unsigned init_marker;
  int flags;
  FILE* out;
  int test_count;
  int failure_count;
  int failing_checks;
} _UT;

#define _ut_die_unless_initialized() \
do {\
  if (_UT.init_marker != _UT_INIT_MARKER) {\
    fprintf(stderr, "UT test framework was not initialized!\n");\
    exit(1);\
  }\
} while (0)

#define _ut_fail(PATH, LINE, MSG) \
do {\
  _UT.failing_checks++;\
  if (_UT.failing_checks < 10) {\
    va_list ap;\
    va_start(ap, MSG);\
    fprintf(_UT.out, "%s:%d: Failing condition: ", PATH, LINE);\
    vfprintf(_UT.out, MSG, ap);\
    fputc('\n', _UT.out);\
    va_end(ap);\
    if (_UT.flags & _UT_FLAGS_ABORT) {\
      abort();\
    }\
  }\
} while (0)

static void UT_start(const char* suite, int flags)
{
  _UT.init_marker = _UT_INIT_MARKER;
  _UT.flags = flags;
  _UT.out = stdout;
  _UT.test_count = 0;
  _UT.failure_count = 0;

  fprintf(_UT.out, "Starting test suite \"%s\" ...\n", suite);
}

static int UT_end(void)
{
  _ut_die_unless_initialized();
  fprintf(_UT.out, "Done (%d of %d test passed)\n", _UT.test_count-_UT.failure_count, _UT.test_count);
  _UT.init_marker = 0;
  return _UT.failure_count;
}

static void UT_expect(const char* path, int line, int condition, const char *msg, ...)
{
  _ut_die_unless_initialized();
  if (!condition) {
    _ut_fail(path, line, msg);
  }
}
#define UT_EXPECT(COND, MSG...) UT_expect(__FILE__, __LINE__, COND, MSG)

static void UT_fail(const char* path, int line, const char* msg, ...)
{
  _ut_die_unless_initialized();
  _ut_fail(path, line, msg);
}
#define UT_FAIL(MSG...) UT_fail(__FILE__, __LINE__, MSG)

static void UT_run(const char* name, void (*test)(void*), void* pass_through)
{
  _ut_die_unless_initialized();
  if (_UT.flags & _UT_FLAGS_VERBOSE) {
    fprintf(_UT.out, "  Testing \"%s\" ...\n", name);
  }
  _UT.failing_checks = 0;
  _UT.test_count++;
  test(pass_through);
  if (_UT.failing_checks) {
    _UT.failure_count++;
  }
}
#define UT_RUN(NAME, PASS_THROUGH) UT_run(#NAME, NAME, (PASS_THROUGH))
#define UT_RUN1(NAME) UT_run(#NAME, NAME, NULL)

#endif /* HEADER_22bf4689_c2e1_4622_9adb_07746882f399 */
