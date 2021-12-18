#include "libio.h"
#include <string.h>
#include <errno.h>
#include <stdarg.h>

void throw_error(const char *fmt, ...){
  va_list args;
  va_start(args, fmt);

  fflush(stdout);

  fprintf(stderr, RED);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  if (errno)
    fprintf(stderr, "\t%s\n", strerror(errno));
  fprintf(stderr, NC);

  exit(0);

  va_end(args);
}

void warning(const char *fmt, ...){
  va_list args;
  va_start(args, fmt);

  printf(MAGENTA);
  vprintf(fmt, args);
  printf("\n%s", NC);

  va_end(args);
}

void success(const char *fmt, ...){
  va_list args;
  va_start(args, fmt);

  printf(GREEN);
  vprintf(fmt, args);
  printf("\n%s", NC);

  va_end(args);
}
