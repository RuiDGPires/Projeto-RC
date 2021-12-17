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

#ifndef NO_ASSERT
  #define ASSERT(expression, ...) \
    do{\
    if (expression);\
    else throw_error(__VA_ARGS__);\
    }while(0)
#else
  #define ASSERT(...) 
#endif


#ifdef DEBUG
  #define PRINT_FUNC_NAME() fprintf(stderr, "%s: ", __FUNCTION_NAME__)

  #define SECTION_MAX_SIZE 5
  static char __debug_msg_section[SECTION_MAX_SIZE] = "NONE";
  static char __debug_colors[2][10] = {YELLOW, CYAN};
  static int __new_section = 0, __color = 0;

  #define DEBUG_MSG_SECTION(...) \
    do {\
      sprintf(__debug_msg_section, __VA_ARGS__);\
      __new_section = 1;\
      __color = !__color; \
    }while(0)
    
  #define DEBUG_MSG(...)\
    do{\
      if (__new_section) {printf("\n"); __new_section = 0;}\
      printf("%s[%s]%s ", __debug_colors[__color], __debug_msg_section, NC); \
      printf(__VA_ARGS__); \
    }while(0)
#else
  #define DEBUG_MSG(...)
  #define DEBUG_MSG_SECTION(...) 
  #define PRINT_FUNC_NAME() 
#endif

#endif
