#include "commands.h"
#include "util.h"
#include "debug.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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

void reg(connection_context_t *connection, char *args){
  char buffer[BUFFER_SIZE];
  char response_buffer[BUFFER_SIZE];
  char *uid, *pass;
  uid = get_word(&args);
  pass = get_word(&args);

  check_uid(uid);
  check_pass(pass);

  sprintf(buffer, "%s %s %s\n", "REG", uid, pass);

  send_udp_message(connection, buffer, response_buffer);

  char *response = response_buffer; // This has to be done for some reason... compiler does not accept casting

  EXPECT(get_word(&response), "RRG");

  char *status = get_word(&response);
  if (strcmp(status, "OK") == 0){
    success("User successfully registered");
  }else if (strcmp(status, "DUP") == 0){
    warning("User is already registered");
  }else {
    warning("An error occured while registering user");
  }
}

void unregister(connection_context_t *connection, char *args){
  char buffer[BUFFER_SIZE];
  char response_buffer[BUFFER_SIZE];
  char *uid, *pass;
  uid = get_word(&args);
  pass = get_word(&args);

  check_uid(uid);
  check_pass(pass);

  sprintf(buffer, "%s %s %s\n", "UNR", uid, pass);

  send_udp_message(connection, buffer, response_buffer);

  char *response = response_buffer;

  EXPECT(get_word(&response), "RUN");

  char *status = get_word(&response);
  if (strcmp(status, "OK") == 0){
    success("User successfully unregistered");
  }else {
    warning("Invalid user or incorrect password");
  }
}

void login_(connection_context_t *connection, char *args){
  char buffer[BUFFER_SIZE];
  char response_buffer[BUFFER_SIZE];
  char *uid, *pass;
  uid = get_word(&args);
  pass = get_word(&args);

  check_uid(uid);
  check_pass(pass);

  sprintf(buffer, "%s %s %s\n", "LOG", uid, pass);
  
  session_context_t *session = connection->session;

  send_udp_message(connection, buffer, response_buffer);

  char *response = response_buffer;

  EXPECT(get_word(&response), "RLO");

  char *status = get_word(&response);
  if (strcmp(status, "OK") == 0){
    if (is_logged(session)){
      warning("You are already logged in");
    }else{
      success("You are now logged in");
      login(session, uid, pass);
    }
  }else {
    warning("Invalid user or incorrect password");
  }
}

void logout_(connection_context_t *connection, char *args){
  char buffer[BUFFER_SIZE];
  char response_buffer[BUFFER_SIZE];
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return; 
  }
  sprintf(buffer, "%s %s %s\n", "OUT", session->uid, session->pass);

  send_udp_message(connection, buffer, response_buffer);

  char *response = response_buffer;

  EXPECT(get_word(&response), "ROU");
  EXPECT(get_word(&response), "OK");

  success("You are now logged out");
  logout(session);
}

void showuid(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return;
  }
  printf("%s\n", session->uid);
}

#define RESPONSE_SIZE BUFFER_SIZE*50
void groups(connection_context_t *connection, char *args){
  char response_buffer[RESPONSE_SIZE];
  send_udp_message_size(connection, "GLS\n", response_buffer, RESPONSE_SIZE);
  
  char *response = response_buffer;

  EXPECT(get_word(&response), "RGL");

  int N = atoi(get_word(&response));

  if (N != 0){
    success("List of groups:");
    char *id, *name;
    for (int i = 0; i < N; i++){
      id = get_word(&response);
      name = get_word(&response);
      printf("\t[%s] %s\n", id, name);
      (void) get_word(&response);
    }
  }else{
    printf("There are no groups available\n");
  }
}

void subscribe(connection_context_t *connection, char *args){
  char *gid = get_word(&args);
  char *gname = get_word(&args);

  if (strcmp(gid, "00") != 0 && strcmp(gid, "0") != 0)
    check_gid(gid);
  int gid_int = atoi(gid);

  char buffer[BUFFER_SIZE];

  sprintf(buffer, "%s %2d %s\n", "GSR", gid_int, gname);
  send_udp_message(connection, buffer, buffer);

  char *response = buffer;
  EXPECT(get_word(&response), "RGS");
  char *status = get_word(&response);

  if (strcmp(status, "OK") == 0){

  }else if (strcmp(status, "NEW") == 0){

  }else if (strcmp(status, "E_USR") == 0){

  }else if (strcmp(status, "E_GRP") == 0){

  }else if (strcmp(status, "E_GNAME") == 0){

  }else if (strcmp(status, "E_FULL") == 0){

  }else{

  }
}

void unsubscribe(connection_context_t *connection, char *args){

}

void my_groups(connection_context_t *connection, char *args){

}

void select_(connection_context_t *connection, char *args){

}

void showgid(connection_context_t *connection, char *args){

}

void ulist(connection_context_t *connection, char *args){

}

