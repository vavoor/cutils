#include "ut.h"
#include "futils.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void test_loading_small_file(void* pt)
{
  char* content;

  int n = FUReadFile("test/lorem.txt", 0, FU_FLAGS_NONE, &content);
  UT_expect(n == 1332, "1332 bytes of lorem.txt are read");
  UT_expect(strncmp(content, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod", 71) == 0, "File contains lorem ipsum");
  free(content);

  n = FUReadFile("test/lorem.txt", -1, FU_FLAGS_NONE, &content);
  UT_expect(n == 1332, "1332 bytes of lorem.txt are read");
  UT_expect(strncmp(content, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod", 71) == 0, "File contains lorem ipsum");
  free(content);

  n = FUReadFile("test/lorem.txt", 70, FU_FLAGS_IGNORE_LARGE_FILES, &content);
  UT_expect(n < 0, "Big file is ignored");
  UT_expect(content == NULL, "in case of ignore, no memory is allocated");
  free(content);

  n = FUReadFile("test/lorem.txt", 71, FU_FLAGS_NONE, &content);
  UT_expect(n == 71, "Max size 71 bytes are read");
  UT_expect(strncmp(content, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod", 71) == 0, "File contains lorem ipsum");
  free(content);
}

void test_loading_large_file(void* pt)
{
  char* content;

  char fname[] = "/tmp/fileXXXXXX";
  int fd = mkstemp(fname);
  assert(fd >= 0);

  FILE* fout = fopen(fname, "w");
  assert(fout != NULL);

  int i;
  for (i = 0; i < (1<< 21); i++) {
    fputc(0x7F & i, fout);
  }
  fclose(fout);
  close(fd);

  int n;
  n = FUReadFile(fname, -1, FU_FLAGS_IGNORE_LARGE_FILES, &content);
  UT_expect(n < 0, "File is larged than default size 1 MiB");

  n = FUReadFile(fname, -1, FU_FLAGS_NONE, &content);
  UT_expect(n == (1<<20), "Default size is 1 MiB");

  free(content);
  remove(fname);
}

int main()
{
  UT_start("futils", _UT_FLAGS_NONE);
  UT_RUN(test_loading_small_file, NULL);
  UT_RUN(test_loading_large_file, NULL);
  return UT_end();
}
