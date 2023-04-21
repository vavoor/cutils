#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "err.h"

static FILE* log_file = NULL;
static int error_count = 0;
static int warning_count = 0;
static int err_flags = 0;

static void output_message(const char* msg_type, const char* msg, va_list ap)
{
  if (!(err_flags & ERR_FLAGS_QUIET)) {
    fprintf(stderr, "%s: ", msg_type);
    vfprintf(stderr, msg, ap);
    fprintf(stderr, ".\n");
  }

  if (log_file != NULL) {
    fprintf(log_file, "%s: ", msg_type);
    vfprintf(log_file, msg, ap);
    fprintf(log_file, ".\n");
  }
}

void ErrInit(const char* log_file_name, int flags)
{
  if (log_file != NULL) {
    fclose(log_file);
  }

  if (log_file_name != NULL && *log_file_name != '\0') {
    log_file = fopen(log_file_name, "a");
    if (log_file == NULL) {
      fprintf(stderr, "Warning: Log file %s cannot be opened for appending.\n", log_file_name);
    }
  }

  err_flags = flags;
  error_count = 0;
  warning_count = 0;
}

void ErrFatal(const char* msg, ...)
{
  va_list ap;
  va_start(ap, msg);
  output_message("Fatal", msg, ap);
  va_end(ap);

  if (err_flags & ERR_FLAGS_ABORT_ON_FATAL) {
    abort();
  }
  exit(1);
}

void ErrError(const char* msg, ...)
{
  va_list ap;
  va_start(ap, msg);
  output_message("Error", msg, ap);
  va_end(ap);
  error_count++;

  if (err_flags & ERR_FLAGS_ABORT_ON_ERROR) {
    abort();
  }
}

void ErrWarning(const char* msg, ...)
{
  va_list ap;
  va_start(ap, msg);
  output_message("Warning", msg, ap);
  va_end(ap);
  warning_count++;
}

int ErrErrorCount(void)
{
  return error_count;
}

int ErrWarningCount(void)
{
  return warning_count;
}
