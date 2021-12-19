/*----------------------------------------------
 *
 * client.c
 * -------
 * Includes the entry point for the application
 * and base functionality
 *
 *---------------------------------------------*/

#include <string.h>
#include <ctype.h>

#include "util.h"
#include "debug.h"
#include "libio.h"
#include "connection.h"

#define DEFAULT_DSIP "localhost"
#define DEFAULT_DSPORT "58065" // 58000 + GN

size_t get_line(char buffer[], FILE *stream){
  fscanf(stream, "%[^\n]", buffer);
  (void) getc(stdin);
  return strlen(buffer);
}

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

char *get_word(char *str[]){
  DEBUG_MSG_SECTION("WRD");
  char command_buffer[BUFFER_SIZE];
  sscanf(*str, "%[^ ]", command_buffer);

  size_t size = strlen(command_buffer);

  (*str)[size-((*str)[size-1] == '\n')] = '\0';

  char *ret = *str;
  *str = &((*str)[size+1]);

  return ret;
}

#define EXPECT_RESPONSE(str, expected) ASSERT(strcmp(str, expected)==0, "Unexpected response from server")

bool parse_input (connection_context_t *context, char str[]){
  char *command = get_word(&str);

  if (strcmp(command, "reg") == 0){
    char buffer[BUFFER_SIZE];
    char response_buffer[BUFFER_SIZE];
    char *uid, *pass;
    uid = get_word(&str);
    pass = get_word(&str);

    check_uid(uid);
    check_pass(pass);

    sprintf(buffer, "%s %s %s\n", "REG", uid, pass);

    send_udp_message(context, buffer, response_buffer);

    char *response = response_buffer; // This has to be done for some reason... compiler does not accept casting

    EXPECT_RESPONSE(get_word(&response), "RRG");

    char *status = get_word(&response);
    if (strcmp(status, "OK") == 0){
      success("User successfully registered");
    }else if (strcmp(status, "DUP") == 0){
      warning("User is already registered");
    }else {
      warning("An error occured while registering user");
    }
  }else if (strcmp(command, "unregister") == 0){
    char buffer[BUFFER_SIZE];
    char response_buffer[BUFFER_SIZE];
    char *uid, *pass;
    uid = get_word(&str);
    pass = get_word(&str);

    check_uid(uid);
    check_pass(pass);

    sprintf(buffer, "%s %s %s\n", "UNR", uid, pass);

    send_udp_message(context, buffer, response_buffer);

    char *response = response_buffer;

    EXPECT_RESPONSE(get_word(&response), "RUN");

    char *status = get_word(&response);
    if (strcmp(status, "OK") == 0){
      success("User successfully unregistered");
    }else {
      warning("Invalid user or incorrect password");
    }
  }else if (strcmp(command, "login") == 0){
    char buffer[BUFFER_SIZE];
    char response_buffer[BUFFER_SIZE];
    char *uid, *pass;
    uid = get_word(&str);
    pass = get_word(&str);

    check_uid(uid);
    check_pass(pass);

    sprintf(buffer, "%s %s %s\n", "LOG", uid, pass);
    
    session_context_t *session = context->session;

    send_udp_message(context, buffer, response_buffer);

    char *response = response_buffer;

    EXPECT_RESPONSE(get_word(&response), "RLO");

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
  }else if (strcmp(command, "logout") == 0){
    char buffer[BUFFER_SIZE];
    char response_buffer[BUFFER_SIZE];
    session_context_t *session = context->session;

    if (!is_logged(session)){
      warning("You are not logged in");
      return 1;
    }
    sprintf(buffer, "%s %s %s\n", "OUT", session->uid, session->pass);

    send_udp_message(context, buffer, response_buffer);

    char *response = response_buffer;

    EXPECT_RESPONSE(get_word(&response), "ROU");
    EXPECT_RESPONSE(get_word(&response), "OK");

    success("You are now logged out");
    logout(session);
  }else if (strcmp(command, "showuid") == 0 || strcmp(command, "su") == 0){
    session_context_t *session = context->session;

    if (!is_logged(session)){
      warning("You are not logged in");
      return 1;
    }
    printf("%s\n", session->uid);
  }else if (strcmp(command, "exit") == 0){
    return 0;
  }else if (strcmp(command, "groups") == 0 || strcmp(command, "gl") == 0){
#define RESPONSE_SIZE BUFFER_SIZE*50
    char response_buffer[RESPONSE_SIZE];
    send_udp_message_size(context, "GLS\n", response_buffer, RESPONSE_SIZE);
    
    char *response = response_buffer;

    EXPECT_RESPONSE(get_word(&response), "RGL");

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
  }else if (strcmp(command, "subscribe") == 0 || strcmp(command, "s") == 0){
  }else if (strcmp(command, "unsubscribe") == 0 || strcmp(command, "u") == 0){
  }else if (strcmp(command, "my_groups") == 0 || strcmp(command, "mgl") == 0){
  }else if (strcmp(command, "select") == 0 || strcmp(command, "sag") == 0){
  }else if (strcmp(command, "showgid") == 0 || strcmp(command, "sg") == 0){
  }else if (strcmp(command, "ulist") == 0 || strcmp(command, "ul") == 0){
  }else if (strcmp(command, "post") == 0){
  }else if (strcmp(command, "retrieve") == 0 || strcmp(command, "r") == 0){
  }else throw_error("Unkown command");

  return 1;
}

#define CLEAR(var) var[0] = '\0'
#define DEFAULT(var, str) if (var[0] == '\0') strcpy(var, str)

void parse_args(char *dsip, char *dsport, int argc, char *argv[]){
  DEBUG_MSG_SECTION("ARGS");
  ASSERT(argc % 2 != 0, "Invalid number of arguments");

  CLEAR(dsip);
  CLEAR(dsport);

  // Parse args
  for (int i = 1; i < argc; i += 2){
    if (strcmp(argv[i], "-n") == 0)
      strcpy(dsip, argv[i+1]);
    else if (strcmp(argv[i], "-p") == 0)
      strcpy(dsport, argv[i+1]);
    else throw_error("Unkown command line argument");
  }

  DEFAULT(dsip, DEFAULT_DSIP);
  DEFAULT(dsport, DEFAULT_DSPORT);

  DEBUG_MSG("dsip set to: %s\n", dsip);
  DEBUG_MSG("dsport set to: %s\n", dsport);
}

int main(int argc, char *argv[]){
  DEBUG_MSG_SECTION("MAIN");
  
  char dsip[BUFFER_SIZE], dsport[BUFFER_SIZE];
  parse_args(dsip, dsport, argc, argv); // Recebe command line arguments e guarda em dsip e dsport

  connection_context_t *context = init_connection(dsip, dsport); // Inicializa conecção e guarda informação em context

  char input_buffer[BUFFER_SIZE];

  int keep_prompt;
  do{
    DEBUG_MSG_SECTION("INPT");
    DEBUG_MSG("Awaiting input...\n");

    size_t size = get_line(input_buffer, stdin);

    keep_prompt = parse_input(context, input_buffer);
  }while(keep_prompt);

  close_connection(&context);
}
