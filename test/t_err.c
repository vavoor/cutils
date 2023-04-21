#include "ut.h"
#include "err.h"

void test_errors(void* pt)
{
  ErrError("This %s an error message", "is");
  UT_expect(ErrErrorCount() == 1, "One error was emitted");
}

void test_warnings(void* pt)
{
    ErrWarning("This %s a warning message", "is");
    UT_expect(ErrWarningCount() == 1, "One warning was emitted");
}

int main()
{
  UT_start("Error messages", _UT_FLAGS_NONE);
  ErrInit(NULL, ERR_FLAGS_QUIET);
  UT_RUN(test_errors, NULL);
  UT_RUN(test_warnings, NULL);
  return UT_end() > 0;
}
