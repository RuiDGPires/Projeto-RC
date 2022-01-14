#include "../common/util.h"
#include "../common/debug.h"
#include "../common/constants.h"
#include "commands.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>


int check_uid(const char str[]){
  ASSERT(str != NULL, FERROR, "Invalid user name");
  
  size_t size = strlen(str);
  ASSERT(size == 5, FERROR, "Invalid user name size");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i])){
      throw_error("Invalid user name characters");
      return FERROR;
    }
  return SUCCESS;
  
}

int check_pass(const char str[]){
  ASSERT(str != NULL, FERROR, "Invalid password");

  size_t size = strlen(str);
  ASSERT(size == 8, FERROR,  "Invalid password size");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i]) && !isalpha(str[i])){
      throw_error("Invalid password characters");
      return FERROR;
    }
  return SUCCESS;
}

int check_gid(const char str[]){
  ASSERT(str != NULL, FERROR, "Invalid group id");

  size_t size = strlen(str);
  ASSERT(size == 2, FERROR, "Invalid group id size");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i])){
      throw_error("Invalid group id chars");
      return FERROR;
    }

  return SUCCESS;
}

int check_gname(const char str[]){
  ASSERT(str != NULL, FERROR, "Invalid group name");
  
  size_t size = strlen(str);
  ASSERT(size < 24, FERROR, "Invalid group name length");

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i]) && !isalpha(str[i]) && str[i] != '-' && str[i] != '_'){
      throw_error("Invalid group name chars");
      return FERROR;
    }
  return SUCCESS;
}

int check_mid(const char str[]){
  ASSERT(str != NULL, FERROR, "Invalid message number");

  size_t size = strlen(str);
  ASSERT(size <= 4 && size >= 1, FERROR, "Invalid message number length");

  //Check for only a space

  for (size_t i = 0; i < size; i++)
    if (!isdigit(str[i])){
      throw_error("Invalid message number chars");
      return FERROR;
    }

  ASSERT(atoi(str) != 0, FERROR, "Invalid message number"); 

  return SUCCESS;
}


int reg(connection_context_t *connection, char *args){
  char *uid = get_word(&args);
  char *pass = get_word(&args);

  if(check_uid(uid) == FERROR || check_pass(pass) == FERROR) return WARNING;

  char buffer[BUFFER_SIZE];
  char response_buffer[BUFFER_SIZE];
  sprintf(buffer, "%s %s %s\n", "REG", uid, pass);

  int rcv_success;
  send_udp_message(connection, buffer, response_buffer,&rcv_success);

  ASSERT(rcv_success == 1, FERROR, "An error occured while connecting to the server");

  char *response = response_buffer; // This has to be done for some reason... compiler does not accept casting

  EXPECT(get_word(&response), "RRG");

  char *status = get_word(&response);
  if (strcmp(status, "OK") == 0){
    success("User successfully registered");
    return SUCCESS;
  }else if (strcmp(status, "DUP") == 0){
    warning("User is already registered");
    return WARNING;
  }else {
    throw_error("An error occured while registering user");
    return WARNING;
  }
}

int unregister(connection_context_t *connection, char *args){
  char *uid = get_word(&args);
  char *pass = get_word(&args);

  if(check_uid(uid) == FERROR || check_pass(pass) == FERROR) return WARNING;

  char buffer[BUFFER_SIZE];
  char response_buffer[BUFFER_SIZE];
  sprintf(buffer, "%s %s %s\n", "UNR", uid, pass);

  int rcv_success;
  send_udp_message(connection, buffer, response_buffer,&rcv_success);

  ASSERT(rcv_success == 1, FERROR, "An error occured while connecting to the server");

  char *response = response_buffer;

  EXPECT(get_word(&response), "RUN");

  char *status = get_word(&response);
  if (strcmp(status, "OK") == 0){
    success("User successfully unregistered");
    if (is_logged(connection->session)){
        if (strcmp(connection->session->uid, uid) == 0)
            logout(connection->session);
    }
    return SUCCESS;
  }else {
    throw_error("Invalid user or incorrect password");
    return WARNING;
  }
}

int login_(connection_context_t *connection, char *args){
  char *uid = get_word(&args);
  char *pass = get_word(&args);

  if(check_uid(uid) == FERROR || check_pass(pass) == FERROR) return WARNING;

  char buffer[BUFFER_SIZE];
  char response_buffer[BUFFER_SIZE];
  sprintf(buffer, "%s %s %s\n", "LOG", uid, pass);
  
  session_context_t *session = connection->session;

  int rcv_success;
  send_udp_message(connection, buffer, response_buffer,&rcv_success);

  ASSERT(rcv_success == 1, FERROR, "An error occured while connecting to the server");

  char *response = response_buffer;

  EXPECT(get_word(&response), "RLO");

  char *status = get_word(&response);
  if (strcmp(status, "OK") == 0){
    if (is_logged(session)){
      warning("You are already logged in");
      return WARNING;
    }else{
      success("You are now logged in");
      login(session, uid, pass);
      return SUCCESS;
    }
  }else {
    throw_error("Invalid user or incorrect password");
    return WARNING;
  }
}

int logout_(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return WARNING;
  }

  char buffer[BUFFER_SIZE];
  char response_buffer[BUFFER_SIZE];
  sprintf(buffer, "OUT %s %s\n",  session->uid, session->pass);

  int rcv_success;
  send_udp_message(connection, buffer, response_buffer, &rcv_success);

  ASSERT(rcv_success == 1, FERROR, "An error occured while connecting to the server");

  char *response = response_buffer;

  EXPECT(get_word(&response), "ROU");
  EXPECT(get_word(&response), "OK");

  success("You are now logged out");
  logout(session);
  return SUCCESS;
}

int showuid(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return WARNING;
  }
  info("%s\n", session->uid);
  return SUCCESS;
}

#define RESPONSE_SIZE BUFFER_SIZE*50
int groups(connection_context_t *connection, char *args){
  char response_buffer[RESPONSE_SIZE];
  int rcv_success;

  char buffer[BUFFER_SIZE];
  sprintf(buffer, "GLS\n");
  send_udp_message_size(connection, buffer, response_buffer, RESPONSE_SIZE, &rcv_success);

  ASSERT(rcv_success == 1, FERROR, "An error occured while connecting to the server");
  
  char *response = response_buffer;

  EXPECT(get_word(&response), "RGL");

  int N = atoi(get_word(&response));

  if (N != 0){
    success("List of groups:");
    char *id, *name;
    for (int i = 0; i < N; i++){
      id = get_word(&response);
      name = get_word(&response);
      info("\t[%s] %s\n", id, name);
      (void) get_word(&response);
    }
  }else{
    warning("There are no groups available\n");
    return WARNING;
  }
  return SUCCESS;
}

int subscribe(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return WARNING;
  }

  char *gid = get_word(&args);
  char *gname = get_word(&args);

  if(check_gid(gid) == FERROR || check_gname(gname) == FERROR) return WARNING;

  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s %s %s %s\n", "GSR", session->uid, gid, gname);

  int rcv_success;
  send_udp_message(connection, buffer, buffer, &rcv_success);

  ASSERT(rcv_success == 1, FERROR, "An error occured while connecting to the server");

  char *response = buffer;
  EXPECT(get_word(&response), "RGS");
  char *status = get_word(&response);

  if (strcmp(status, "OK") == 0){
    success("Subscribed to: [%s] %s", gid, gname);
    return SUCCESS;
  }else if (strcmp(status, "NEW") == 0){
    success("New group create and subscribed: [%s] %s", get_word(&response), gname);
    return SUCCESS;
  }else if (strcmp(status, "E_USR") == 0){
    throw_error("Invalid UID");
    return WARNING;
  }else if (strcmp(status, "E_GRP") == 0){
    throw_error("Invalid GID: %s", gid);
    return WARNING;
  }else if (strcmp(status, "E_GNAME") == 0){
    throw_error("Invalid GNAME: %s", gname);
    return WARNING;
  }else if (strcmp(status, "E_FULL") == 0){
    warning("Server capacity is full, no more groups can be created");
    return WARNING;
  }else{
    throw_error("Unkown error");
    return WARNING;
  }
}

int unsubscribe(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return WARNING;
  }

  char *gid = get_word(&args);

  if (check_gid(gid) == FERROR) return WARNING;

  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s %s %s\n", "GUR", session->uid, gid);

  int rcv_success;
  send_udp_message(connection, buffer, buffer, &rcv_success);

  ASSERT(rcv_success == 1, FERROR, "An error occured while connecting to the server");

  char *response = buffer;

  EXPECT(get_word(&response), "RGU");
  char *status = get_word(&response);

  if (strcmp(status, "OK") == 0){
    success("You are no longer subscribed to group %s", gid);
    return SUCCESS;
  }else if (strcmp(status, "E_USR") == 0){
    throw_error("Invalid UID");
    return WARNING;
  }else if (strcmp(status, "E_GRP") == 0){
    throw_error("Invalid GID: %s", gid);
    return WARNING;
  }else {
    throw_error("Unkown error");
    return WARNING;
  }
}

int my_groups(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return WARNING;
  }

  char buffer[RESPONSE_SIZE];

  int rcv_success;
  sprintf(buffer, "%s %s\n", "GLM", session->uid);
  send_udp_message_size(connection, buffer, buffer, RESPONSE_SIZE, &rcv_success);

  ASSERT(rcv_success == 1, FERROR, "An error occured while connecting to the server");

  char *response = buffer;

  EXPECT(get_word(&response), "RGM");

  char *N_str = get_word(&response); 
  
  if (strcmp(N_str, "E_USR") == 0){
    throw_error("Invalid UID");
    return WARNING;
  }else{
    int N = atoi(N_str);

    if (N != 0){
      success("Groups you are subscribed to:");
      char *id, *name;
      for (int i = 0; i < N; i++){
        id = get_word(&response);
        name = get_word(&response);
        info("\t[%s] %s\n", id, name);
        (void) get_word(&response);
      }
      return SUCCESS;
    }else{
      warning("You are not subscribed to any group\n");
      return WARNING;
    }
  }
}

int select_(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  char *gid = get_word(&args);

  if (check_gid(gid) == FERROR) return WARNING;

  if (!is_logged(session)){
    warning("You are not logged in");
    return WARNING;
  }

  select_group(connection->session, gid);
  success("Selected: %s", session->gid);
  return SUCCESS;
}

int showgid(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return WARNING;
  }
  if (!is_group_selected(session)){
    warning("No group selected");
    return WARNING;
  }

  info("Selected: %s\n", session->gid);
  return SUCCESS;
}

int ulist(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return WARNING;
  }

  if (!is_group_selected(session)){
    warning("No group selected");
    return WARNING;
  }

  char buffer[BUFFER_SIZE];
  sprintf(buffer, "%s %s\n", "ULS", session->gid);
  send_tcp_message_no_answer(connection, buffer);


  get_word_fd(connection->tcp_info->fd, buffer);
  EXPECT(buffer, "RUL");

  get_word_fd(connection->tcp_info->fd, buffer);

  if (strcmp(buffer, "OK") == 0){
    get_word_fd(connection->tcp_info->fd, buffer);

    char gname[BUFFER_SIZE];
    strcpy(gname, buffer);

    if (buffer[0] == '\0'){
      warning("There are no users subscribed to %s", gname);
      return WARNING;
    }else{
      success("List of users subscribed to %s:", gname);

      while(buffer[0] != '\0'){
        get_word_fd(connection->tcp_info->fd, buffer);
        info("\t%s\n", buffer);
      }
      return SUCCESS;
    }
  }else {
    warning("Group %s does not exist", session->gid);
    return WARNING;
  }
}

int post(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return WARNING;
  }

  if (!is_group_selected(session)){
    warning("No group selected");
    return WARNING;
  }

  char *msg = get_quote(&args);
  size_t msg_size = strlen(msg);

  // Verificar aspas
  ASSERT(msg[msg_size - 1] == '"', WARNING, "Message must be surrounded by double quotes");
  ASSERT(msg[0] == '"', WARNING, "Message must be surrounded by double quotes");

  // Remover aspas
  msg[msg_size-1] = '\0';
  msg = &msg[1];

  msg_size -= 2;
  
  char *file_name = args != NULL ? get_word(&args): NULL;

  char *buffer; 

  size_t sending_msg_size = 0;
  if (!file_name){
    buffer = (char *) malloc(sizeof(char) * (msg_size + BUFFER_SIZE));
    
    sprintf(buffer, "PST %s %s %ld %s\n", session->uid, session->gid, msg_size, msg);
    sending_msg_size = strlen(buffer);
  }else {
    // Abrir ficheiro
    FILE *file = fopen(file_name, "rb");
    ASSERT(file != NULL, WARNING, "Unable to open file: %s", file_name);

    size_t file_size = get_file_size(file);

    buffer = (char *) malloc(sizeof(char) * (msg_size + BUFFER_SIZE + file_size));

    sprintf(buffer, "PST %s %s %ld %s %s %ld ", session->uid, session->gid, msg_size, msg, file_name, file_size);
    
    sending_msg_size = strlen(buffer) + file_size + 1;
    read_file(file, file_size, &buffer[strlen(buffer)]);

    fclose(file);
  }

  char response_buffer[BUFFER_SIZE];
  send_tcp_message_sending_size(connection, buffer, response_buffer, sending_msg_size);

  free(buffer);

  char *response = response_buffer;
  EXPECT(get_word(&response), "RPT");

  char *status = get_word(&response);

  if (strcmp(status, "NOK") == 0){
    throw_error("Post failed");
    return WARNING;
  }else{
    success("Post succeeded with MID: %s", status);
    return SUCCESS;
  }
}


int retrieve(connection_context_t *connection, char *args){
  session_context_t *session = connection->session;

  if (!is_logged(session)){
    warning("You are not logged in");
    return WARNING;
  }

  if (!is_group_selected(session)){
    warning("No group selected");
    return WARNING;
  }

  char *mid = get_word(&args);

  if(check_mid(mid) == FERROR) return WARNING;

  int mid_int = atoi(mid);
  char buffer[BUFFER_SIZE];
  sprintf(buffer, "RTV %s %s %04d\n", session->uid, session->gid, mid_int);

  send_tcp_message_no_answer(connection, buffer);

  get_word_fd(connection->tcp_info->fd, buffer);

  EXPECT(buffer, "RRT");

  get_word_fd(connection->tcp_info->fd, buffer);

  DEBUG_MSG_SECTION("RET");
  DEBUG_MSG("Reponse: RRT %s\n", buffer);
  if (strcmp(buffer, "OK") == 0){
    get_word_fd(connection->tcp_info->fd, buffer);
    DEBUG_MSG("N = %s\n", buffer);
    int N = atoi(buffer);
    success("%d message(s) retrieved:", N);

    get_word_fd(connection->tcp_info->fd, buffer);
    for (int i = 0; i < N; i ++){
      // Get message ID
      char mid[5];
      strcpy(mid, buffer);
      mid[4] = '\0';

      // Get user ID
      get_word_fd(connection->tcp_info->fd, buffer);
      char uid[6];
      strcpy(uid, buffer);
      uid[5] = '\0';

      // Get message size
      get_word_fd(connection->tcp_info->fd, buffer);
      int msg_size = atoi(buffer); 

      DEBUG_MSG("Message size: %d\n", msg_size);
      // Get message
      char *msg = (char *) malloc(sizeof(char)*msg_size + 1);
      size_t total_read_size = 0;
      while(total_read_size != msg_size)
        total_read_size += read(connection->tcp_info->fd, &msg[total_read_size], msg_size-total_read_size);

      msg[msg_size] = '\0';
      DEBUG_MSG("Text from message: %s\n", msg);

      // get rid of trailing ' '
      (void) read(connection->tcp_info->fd, buffer, 1);

      bool has_file = 0;
      char file_name[BUFFER_SIZE];
  
      // If file appended, get its name
      get_word_fd(connection->tcp_info->fd, buffer);
      DEBUG_MSG("%s\n", buffer);
       
      if (strcmp(buffer, "/") == 0){
        has_file = TRUE;
        FILE *file = NULL;

        get_word_fd(connection->tcp_info->fd, buffer);
        strcpy(file_name, buffer);

        get_word_fd(connection->tcp_info->fd, buffer);
        int file_size = atoi(buffer);

        char *file_data = (char *) malloc(sizeof(char)*file_size);

        size_t total_read_size = 0;

        while(total_read_size != file_size){
          total_read_size += read(connection->tcp_info->fd, &file_data[total_read_size], file_size-total_read_size);
        }

        DEBUG_MSG("File size: %d\n", file_size);
        DEBUG_MSG("Read size %ld\n", total_read_size);

        ASSERT(file_size == total_read_size, WARNING, "File sizes don't match");

        // get rid of ' '
        (void) read(connection->tcp_info->fd, buffer, 1);

        file = fopen(file_name, "wb");

        fwrite(file_data, 1, file_size, file);

        ASSERT(file_size == get_file_size(file), WARNING, "File sizes don't match");
        fclose(file);

        free(file_data);
        get_word_fd(connection->tcp_info->fd, buffer);
      }
      
      info("%s - \"%s\";", mid, msg);
      if (has_file)
        info(" file stored: %s", file_name);
      info("\n");

      free(msg);
    }
  }else if (strcmp(buffer, "EOF") == 0){
    warning("There are no messages available");
    return WARNING;
  }else{ // NOK
    throw_error("Unkown error");
    return WARNING;
  }

  close_tcp_connection(&connection->tcp_info); //Shouldn't we close tcp connection right after we fill the buffer?
  return SUCCESS;
}
