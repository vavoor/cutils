#ifndef HEADER_5e1a8af3_8688_4147_a8e0_dc323a116674
#define HEADER_5e1a8af3_8688_4147_a8e0_dc323a116674

#define ERR_FLAGS_NONE 0x0  /* No flag set */
#define ERR_FLAGS_QUIET 0x01 /* Don write message to stderr */
#define ERR_FLAGS_ABORT_ON_FATAL 0x10 /* Call abort() on fatal instead of exit(1) */
#define ERR_FLAGS_ABORT_ON_ERROR 0x20 /* Call abort() on error */

/**
 * By default, messages are written to stderr. This can be overriden by
 * specifying a file name for <log_file>. NULL disables writing to a log
 * file. Note that the log file is created unless it exists already and
 * messages are appended to the log file.
 *
 * The flag ERR_FLAGS_QUIET suppresses writing the message to stderr.
 */
void ErrInit(const char* log_file, int flags);

/**
 * Output the message tagged as fatal to stderr and/or the log file and
 * terminate the execution of the program with exit(1). See printf for
 * message and parameter handling.
 *
 * The flag ERR_FLAGS_ABORT_ON_FATAL causes aborting the execution with
 * a call to abort(). This can be caught in the debugger.
 */
void ErrFatal(const char* msg, ...);

/**
 * Output the message tagged as error to stderr and/or the log file.
 * See printf for message and parameter handling.
 *
 * The flag ERR_FLAGS_ABORT_ON_ERROR causes aborting the execution with
 * a call to abort() in case of an error. This can be caught in the debugger.
 */
void ErrError(const char* msg, ...);

/**
 * Output the message tagged as warning to stderr and/or the log file.
 * See printf for message and parameter handling.
 */
void ErrWarning(const char* msg, ...);

/**
 * Return the number of calls to ErrError() since the last ErrInit() or
 * program start.
 */
int ErrErrorCount(void);

/**
 * Return the number of calls to ErrWarning() since the last ErrInit() or
 * program start.
 */

int ErrWarningCount(void);

#endif /* HEADER_5e1a8af3_8688_4147_a8e0_dc323a116674 */
