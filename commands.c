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
    warning("There are no groups available\n");
  }
}

void subscribe(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return;
  }

  char *gid = get_word(&args);
  char *gname = get_word(&args);

  char buffer[BUFFER_SIZE];

  sprintf(buffer, "%s %s %s %s\n", "GSR", session->uid, gid, gname);
  send_udp_message(connection, buffer, buffer);

  char *response = buffer;
  EXPECT(get_word(&response), "RGS");
  char *status = get_word(&response);

  if (strcmp(status, "OK") == 0){
    success("Subscribed to: [%s] %s", gid, gname);
  }else if (strcmp(status, "NEW") == 0){
    success("New group create and subscribed: [%s] %s", get_word(&response), gname);
  }else if (strcmp(status, "E_USR") == 0){
    throw_error("Invalid UID"); // This shouldn't happen...
  }else if (strcmp(status, "E_GRP") == 0){
    warning("Invalid GID: %s", gid); 
  }else if (strcmp(status, "E_GNAME") == 0){
    warning("Invalid GNAME: %s", gname);
  }else if (strcmp(status, "E_FULL") == 0){
    warning("Server capacity is full, no more groups can be created");
  }else{
    throw_error("Unkown error");
  }
}

void unsubscribe(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return;
  }

  char *gid = get_word(&args);
  char buffer[BUFFER_SIZE];

  sprintf(buffer, "%s %s %s\n", "GUR", session->uid, gid);
  send_udp_message(connection, buffer, buffer);

  char *response = buffer;

  EXPECT(get_word(&response), "RGU");
  char *status = get_word(&response);

  if (strcmp(status, "OK") == 0){
    success("You are no longer subscribed to group %s", gid);
  }else if (strcmp(status, "E_USR") == 0){
    throw_error("Invalid UID"); // This shouldn't happen...
  }else if (strcmp(status, "E_GRP") == 0){
    warning("Invalid GID: %s", gid);
  }else {
    throw_error("Unkown error");
  }
}

void my_groups(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return;
  }

  char buffer[RESPONSE_SIZE];
  sprintf(buffer, "%s %s\n", "GLM", session->uid);
  send_udp_message_size(connection, buffer, buffer, RESPONSE_SIZE);

  char *response = buffer;

  EXPECT(get_word(&response), "RGM");

  char *N_str = get_word(&response); 
  
  if (strcmp(N_str, "E_USR") == 0){
    throw_error("Invalid UID"); // This shouldn't happen...
  }else{
    int N = atoi(N_str);

    if (N != 0){
      success("Groups you are subscribed to:");
      char *id, *name;
      for (int i = 0; i < N; i++){
        id = get_word(&response);
        name = get_word(&response);
        printf("\t[%s] %s\n", id, name);
        (void) get_word(&response);
      }
    }else{
      warning("You are not subscribed to any group\n");
    }
  }
}

void select_(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return;
  }

  select_group(connection->session, get_word(&args));
  success("Selected: %s", session->gid);
}

void showgid(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return;
  }
  if (!is_group_selected(session)){
    warning("No group selected");
    return;
  }

  printf("Selected: %s\n", session->gid);
}

void ulist(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return;
  }

  if (!is_group_selected(session)){
    warning("No group selected");
    return;
  }

  char buffer[RESPONSE_SIZE];

  sprintf(buffer, "%s %s\n", "ULS", session->gid);
  send_tcp_message_size(connection, buffer, buffer, RESPONSE_SIZE);

  char *response = buffer;
  EXPECT(get_word(&response), "RUL");

  char *status = get_word(&response);

  if (strcmp(status, "OK") == 0){
    char *gname = get_word(&response);
    if (!response){
      warning("There are no users subscribed to %s", gname);
    }else{
      success("List of users subscribed to %s:", gname);

      while(response){
        printf("\t%s\n", get_word(&response));
      }
    }
  }else {
    warning("Group %s does not exist", session->gid);
  }
}

void post(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return;
  }

  if (!is_group_selected(session)){
    warning("No group selected");
    return;
  }

  char *msg = get_quote(&args);
  size_t msg_size = strlen(msg);

  // Verificar aspas
  ASSERT(msg[msg_size - 1] == '"', "Message must be surrounded by double quotes");
  ASSERT(msg[0] == '"', "Message must be surrounded by double quotes");

  // Remover aspas
  msg[msg_size-1] = '\0';
  msg = &msg[1];

  msg_size -= 2;
  
  char *file_name = args != NULL ? get_word(&args): NULL;

  char *buffer; 

  if (!file_name){
    buffer = (char *) malloc(sizeof(char) * (msg_size + BUFFER_SIZE));
    
    sprintf(buffer, "PST %s %s %d %s\n", session->uid, session->gid, msg_size, msg);
  }else {
    // Abrir ficheiro
    FILE *file = fopen(file_name, "r");
    ASSERT(file != NULL, "Unable to open file: %s", file_name);

    size_t file_size = get_file_size(file);

    buffer = (char *) malloc(sizeof(char) * (msg_size + BUFFER_SIZE + file_size));

    sprintf(buffer, "PST %s %s %d %s %s %ld ", session->uid, session->gid, msg_size, msg, file_name, file_size);
    
    read_file(file, file_size, &buffer[strlen(buffer)]);

    fclose(file);
  }

  char response_buffer[BUFFER_SIZE];
  send_tcp_message(connection, buffer, response_buffer);

  free(buffer);

  char *response = response_buffer;
  EXPECT(get_word(&response), "RPT");

  char *status = get_word(&response);

  if (strcmp(status, "NOK") == 0){
    warning("Post failed");
  }else{
    success("Post succeeded with MID: %s", status);
  }
}

void retrieve(connection_context_t *connection, char *args){

}
