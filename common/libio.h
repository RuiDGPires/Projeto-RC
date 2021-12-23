/*----------------------------------------------
 *
 * libio.h
 * -------
 * Library to print formated messages to the user
 *
 *---------------------------------------------*/

#ifndef LIB_IO_H__
#define LIB_IO_H__

#include <stdio.h>
#include <stdlib.h>
#include "util.h"

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define NC "\033[0m" 

void __attribute__((format(printf, 1, 2))) throw_error(const char *fmt, ...);
void __attribute__((format(printf, 1, 2))) warning(const char *fmt, ...);
void __attribute__((format(printf, 1, 2))) success(const char *fmt, ...);

#endif
