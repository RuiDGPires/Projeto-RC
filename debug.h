#ifndef DEBUG_H__
#define DEBUG_H__

#include "libio.h"

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

