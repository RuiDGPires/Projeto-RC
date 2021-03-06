/*----------------------------------------------
 *
 * util.h
 * -------
 * Library with some utility
 * Maybe will be deleted later
 *
 *---------------------------------------------*/

#ifndef UTIL_H__
#define UTIL_H__

#include"constants.h"

#include <stdio.h>
#include <stdlib.h>


typedef char bool; // smallest size


#ifdef DEBUG
  #ifndef __FUNCTION_NAME__ 
    #ifdef WIN32   //WINDOWS
      #define __FUNCTION_NAME__   __FUNCTION__  
    #else          //*NIX
      #define __FUNCTION_NAME__   __func__ 
    #endif
#endif
#endif



size_t get_line(char *, FILE *);
char *get_word(char **);
size_t get_word_fd(int , char *);
void read_fd(int , char *, size_t);
char *get_quote(char **);

size_t get_file_size(FILE *);
size_t read_file(FILE *, size_t, char *);

#endif

