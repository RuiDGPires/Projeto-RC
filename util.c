#include "util.h"
#include <string.h>

char *get_word(char *str[]){
  char command_buffer[BUFFER_SIZE];
  sscanf(*str, "%[^ ]", command_buffer);

  size_t size = strlen(command_buffer);

  (*str)[size-((*str)[size-1] == '\n')] = '\0';

  char *ret = *str;
  *str = &((*str)[size+1]);

  return ret;
}
