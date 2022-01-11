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
#include "constants.h"



void __attribute__((format(printf, 1, 2))) throw_error(const char *fmt, ...);
void __attribute__((format(printf, 1, 2))) warning(const char *fmt, ...);
void __attribute__((format(printf, 1, 2))) success(const char *fmt, ...);
void __attribute__((format(printf, 1, 2))) info(const char *fmt, ...);

#endif
