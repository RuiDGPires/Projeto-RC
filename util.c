#include "util.h"
#include <string.h>

char *get_word(char *str[]){
  int end = 0;
  char command_buffer[BUFFER_SIZE];
  sscanf(*str, "%[^ ]", command_buffer);

  size_t size = strlen(command_buffer);


  if ((*str)[size] == '\0') end = 1;

  (*str)[size-((*str)[size-1] == '\n')] = '\0';

  char *ret = *str;

  if (end) *str = NULL;
  else  *str = &((*str)[size+1]);

  return ret;
}
