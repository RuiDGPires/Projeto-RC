#include "../common/util.h"
#include "../common/debug.h"
#include "commands.h"
#include "file_management.h"
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

// returns:
//  0 if everything is correct
//  1 if user name does not exist
//  2 if password is wrong
int check_credencials(const char name[], const char pass[], const char fs[]){
  int ret = 0;

  char *user_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(name)) + 1);
  sprintf(user_path, "%s/%s/%s", fs, SERVER_USERS_NAME, name);

  if (!(directory_exists(user_path))){
    ret = 1;
  }else{
    char *pass_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(name)) + strlen("pass.txt") + 1);

    sprintf(pass_path, "%s/%s/%s/pass.txt", fs, SERVER_USERS_NAME, name);

    FILE *file = fopen(pass_path, "r");

    char user_pass[9];
    fscanf(file, "%s", user_pass);
    fclose(file);

    DEBUG_MSG_SECTION("UNR");

    if (strcmp(user_pass, pass) == 0){
      ret = 0;
    }else{
      ret = 2;
    }

    free(pass_path);
  }

  free(user_path);
  return ret;
}

void reg(connection_context_t *connection, char *args, char *fs){
  char *name = get_word(&args);
  char *pass= get_word(&args);

  char buffer[BUFFER_SIZE];

  char *user_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(name)) + 1);

  sprintf(user_path, "%s/%s/%s", fs, SERVER_USERS_NAME, name);

  if (directory_exists(user_path)){
    sprintf(buffer, "RRG DUP\n");
  }else{
    create_directory_abs(user_path);
    create_file(user_path, "pass.txt", pass);
    sprintf(buffer, "RRG OK\n");
  }

  send_udp_message(connection, buffer, 7);

  free(user_path);
}

void unregister(connection_context_t *connection, char *args, char *fs){
  char *name = get_word(&args);
  char *pass= get_word(&args);

  char buffer[BUFFER_SIZE];

  char *user_path = (char *) malloc(sizeof(char)*(strlen(fs) + strlen(SERVER_USERS_NAME) + strlen(name)) + 1);

  sprintf(user_path, "%s/%s/%s", fs, SERVER_USERS_NAME, name);

  if (check_credencials(name, pass, fs) == 0){
    delete_directory(user_path);
    sprintf(buffer, "RUN OK\n");
  }else{
    sprintf(buffer, "RUN NOK\n");
  }

  send_udp_message(connection, buffer, 7);
  free(user_path);
}

void login_(connection_context_t *connection, char *args, char *fs){
  char *name = get_word(&args);
  char *pass= get_word(&args);

  char buffer[BUFFER_SIZE];

  if (check_credencials(name, pass, fs) == 0){
    sprintf(buffer, "RLO OK\n");
  }else{
    sprintf(buffer, "RLO NOK\n");
  }

  send_udp_message(connection, buffer, 7);
}

void logout_(connection_context_t *connection, char *args, char *fs){
  char *name = get_word(&args);
  char *pass= get_word(&args);

  char buffer[BUFFER_SIZE];

  if (check_credencials(name, pass, fs) == 0){
    sprintf(buffer, "ROU OK\n");
  }else{
    sprintf(buffer, "ROU NOK\n");
  }

  send_udp_message(connection, buffer, 7);
}

void groups(connection_context_t *, char *, char *){}
void subscribe(connection_context_t *, char *, char *){}
void unsubscribe(connection_context_t *, char *, char *){}
void my_groups(connection_context_t *, char *, char *){}
void ulist(connection_context_t *, char *, char *){}
void post(connection_context_t *, char *, char *){}
void retrieve(connection_context_t *, char *, char *){}
