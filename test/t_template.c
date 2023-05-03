#include "ut.h"
#include <stdio.h>
#include <string.h>
#include "template.h"

char buffer[1<<16];

void print_to_buffer(int c, void* pt)
{
  char** t = (char**) pt;
  **t = c;
  (*t)++;
  **t = '\0';
}

void test_replacements(void* pt)
{
  char template[] = "Say %{greeting}% to %{the world}%\n";
  char* p = buffer;
  template_print_filter(print_to_buffer, &p, template, "greeting", "Hello", "the world", "Gerd", NULL);
  UT_expect(strcmp(buffer, "Say Hello to Gerd\n")==0, "Replaces defined names");
}

int main()
{
  UT_start("Templating engine", 0);
  UT_RUN(test_replacements, NULL);
  return UT_end();
}
