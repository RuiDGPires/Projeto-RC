#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define CYAN "\033[0;36m"
#define NC "\033[0m" 

#ifdef DEBUG
#ifndef __FUNCTION_NAME__ 
	#ifdef WIN32   //WINDOWS
		#define __FUNCTION_NAME__   __FUNCTION__  
	#else          //*NIX
		#define __FUNCTION_NAME__   __func__ 
	#endif
#endif

#define EXTRA() fprintf(stderr, "%s: ", __FUNCTION_NAME__)
#else
#define EXTRA() ;
#endif

#define THROW_ERROR(...) \
  do{\
    fflush(stdout);\
    EXTRA(); \
    fprintf(stderr, RED);\
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n");\
    if (errno)\
      fprintf(stderr, "\t%s\n", strerror(errno));\
    fprintf(stderr, NC);\
    exit(-1);\
  }while(0)

#define ASSERT(expression, ...) \
  do{\
  if (expression);\
  else THROW_ERROR(__VA_ARGS__);\
  }while(0)


#ifdef DEBUG
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
#endif

#endif
