#include "../common/util.h"
#include "../common/debug.h"
#include "commands.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

void check_uid(const char str[]){
  size_t size = strlen(str);
  ASSERT(size == 5, "Invalid user name size");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i])) throw_error("Invalid user name characters");
}

void check_pass(const char str[]){
  size_t size = strlen(str);
  ASSERT(size == 8, "Invalid password size");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i]) && !isalpha(str[i])) throw_error("Invalid password characters");
}

void check_gid(const char str[]){
  size_t size = strlen(str);
  ASSERT(size == 2, "Invalid group number size");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i])) throw_error("Invalid group id chars");
}

void check_gname(const char str[]){
  size_t size = strlen(str);
  ASSERT(size < 24, "Invalid group name length");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i]) && !isalpha(str[i]) && str[i] != '-' && str[i] != '_') throw_error("Invalid group name chars");
}

void reg(connection_context_t *connection, char *args, char *fs){
  send_udp_message(connection, "RRG OK\n", 7);
}
