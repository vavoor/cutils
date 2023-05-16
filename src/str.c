#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include "str.h"

int StrEndsWith(const char* haystack, ... /* const char* needles */)
{
  assert(haystack != NULL);

  va_list ap;
  va_start(ap, haystack);
  int found = 0;
  int len = strlen(haystack);
  const char* needle = va_arg(ap, const char*);
  while (needle != NULL) {
    int n = len - strlen(needle);
    if (n >= 0 && strcmp(&haystack[n], needle) == 0) {
      found = 1;
      goto exit;
    }
    needle = va_arg(ap, const char*);
  }

exit:
  va_end(ap);
  return found;
}
