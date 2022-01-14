#include "util.h"
#include <unistd.h>
#include <string.h>

/*
 * Gets text until the next line
 */
size_t get_line(char buffer[], FILE *stream){
  fscanf(stream, "%[^\n]", buffer);
  (void) getc(stdin);
  return strlen(buffer);
}

/*
 * Updates pointer of buffer to the next word
 * Returns each words every time its called
 * E.g.:
 * bufffer=hello word
 * first call of function returns char* with 'hello' and updates pointer to position of w
 * second call of function returns char* with 'world' and since it has '\n' assumes end of line
 */
char *get_word(char *str[]){
  size_t size = 0;
  int end = 0;

  if ( *str == NULL) return NULL;

  if ( *str[size] == ' '){
    char *ret = *str;
    return ret;
  }

  for (; (*str)[size] != ' ' && (*str)[size] != '\0' && (*str)[size] != '\n' ; size++);

  if ((*str)[size] == '\0') end = 1;

  (*str)[size-((*str)[size-1] == '\n')] = '\0';

  char *ret = *str;

  //updates pointer to next word
  if (end) *str = NULL;
  else  *str = &((*str)[size+1]);

  return ret;

}

/*
 * Reads until it finds a spaces (end of word) or '\n' (end of line).
 * Adds '\0' since read doesn't add it at the end.
 */
size_t get_word_fd(int fd, char str[]){
  size_t size = 0;

  for (; read(fd, &str[size], 1) != 0; size++) if (str[size] == ' ' || str[size] == '\0' || str[size] == '\n' ) break;

  str[size] = '\0';

  return size;
}

void read_fd(int fd, char str[], size_t size){
    if (str != NULL){
        for (size_t current_size = 0; current_size < size ;) current_size += read(fd, &str[current_size], size - current_size);
    }else{
        char c;
        for (size_t current_size = 0; current_size < size ;) current_size += read(fd, &c, 1);
    }

}

/*
 * Gets text inside of air quotes
 */
char *get_quote(char *str[]){
  if ( *str == NULL) return NULL;
  int end = 0;

  size_t size = 1;
  for (; (*str)[size] != '"' && (*str)[size] != '\0'; size++);

  if ((*str)[size + 1] == '\0') end = 1;

  (*str)[size + 1] = '\0';

  char *ret = *str;

  if (end) *str = NULL;
  else  *str = &((*str)[size+2]);

  return ret;
}

/*
 * Gets size of given file
 */
size_t get_file_size(FILE *file){
  // Missing error checking
  fseek(file, 0, SEEK_END); 
  size_t size = ftell(file); 
  fseek(file, 0, SEEK_SET); 
  return size;
}

/*
 * Reads tezt of given file 
 */
size_t read_file(FILE *file, size_t size, char *buffer){
  size_t read_size = fread(buffer, size, 1, file);

  // Puts a \n at the end
  buffer[size] = '\n';
  return read_size;
}
